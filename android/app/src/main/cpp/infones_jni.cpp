/*===================================================================*/
/*                                                                   */
/*  infones_jni.cpp : Java ↔ C++ 桥                                   */
/*                                                                   */
/*  职责：                                                            */
/*  - Surface → ANativeWindow → Port_SetSurface                      */
/*  - SAF 读入的 ROM 字节 → Port_SetRomImage → InfoNES_Load          */
/*  - 模拟线程的创建 / 停止（用 PAD_SYS_QUIT 帧尾优雅退出，不用强杀） */
/*  - 手柄位、跳帧、静音的实时调整                                    */
/*                                                                   */
/*  对应 Java 类：com.infones.app.NativeBridge                       */
/*                                                                   */
/*===================================================================*/

#include <jni.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <android/log.h>

#include <cstdlib>
#include <cstring>
#include <string>
#include <thread>
#include <mutex>

#include "InfoNES.h"                 /* InfoNES_Load / InfoNES_Main / FrameSkip / APU_Mute */
#include "InfoNES_Port_Android.h"

#define LOG_TAG "InfoNES"
#define LOGE(...) __android_log_print( ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__ )

namespace
{

std::mutex  g_thread_mutex;
std::thread *g_thread = nullptr;     /* 模拟线程（运行 InfoNES_Main） */

void emulation_thread_func()
{
  /* Init → Menu → Cycle → …（停止标志生效后）→ Fin，全程内核自治 */
  InfoNES_Main();
}

std::string jstring_to_std( JNIEnv *env, jstring s )
{
  if ( s == nullptr )
    return std::string();
  const char *p = env->GetStringUTFChars( s, nullptr );
  std::string out( p == nullptr ? "" : p );
  env->ReleaseStringUTFChars( s, p );
  return out;
}

} /* namespace */

/*-------------------------------------------------------------------*/
/*  初始化：设置应用私有目录（存档所在）                              */
/*-------------------------------------------------------------------*/

extern "C" JNIEXPORT void JNICALL
Java_com_infones_app_NativeBridge_nativeInit( JNIEnv *env, jclass, jstring files_dir )
{
  Port_SetFilesDir( jstring_to_std( env, files_dir ) );
}

/*-------------------------------------------------------------------*/
/*  画面                                                             */
/*-------------------------------------------------------------------*/

extern "C" JNIEXPORT void JNICALL
Java_com_infones_app_NativeBridge_nativeSetSurface( JNIEnv *env, jclass, jobject surface )
{
  if ( surface == nullptr )
    return;
  /* fromSurface 的引用由端口层接管（Port_ClearSurface 时释放） */
  ANativeWindow *window = ANativeWindow_fromSurface( env, surface );
  if ( window == nullptr )
    return;
  Port_SetSurface( window );
}

extern "C" JNIEXPORT void JNICALL
Java_com_infones_app_NativeBridge_nativeClearSurface( JNIEnv *, jclass )
{
  Port_ClearSurface();
}

/*-------------------------------------------------------------------*/
/*  装载 ROM（字节来自 SAF 的 ContentResolver）                        */
/*-------------------------------------------------------------------*/

extern "C" JNIEXPORT jint JNICALL
Java_com_infones_app_NativeBridge_nativeLoadRom( JNIEnv *env, jclass,
                                                 jbyteArray data, jstring name )
{
  /* 模拟运行中禁止装载（ReleaseRom 会拆掉正在使用的银行） */
  {
    std::lock_guard<std::mutex> lock( g_thread_mutex );
    if ( g_thread != nullptr )
      return -2;
  }

  const jsize size = env->GetArrayLength( data );
  if ( size <= 0 )
    return -1;

  jboolean is_copy = JNI_FALSE;
  jbyte *elems = env->GetByteArrayElements( data, &is_copy );
  if ( elems == nullptr )
    return -1;

  uint8_t *copy = (uint8_t *)malloc( (size_t)size );
  if ( copy == nullptr )
  {
    env->ReleaseByteArrayElements( data, elems, JNI_ABORT );
    return -1;
  }
  memcpy( copy, elems, (size_t)size );
  env->ReleaseByteArrayElements( data, elems, JNI_ABORT );

  Port_SetRomImage( copy, (size_t)size );
  Port_SetRomName( jstring_to_std( env, name ) );

  /* InfoNES_Load：ReleaseRom → ReadRom（内存解析）→ Reset（含 pAPUInit/SoundOpen） */
  const int result = InfoNES_Load( "" );
  if ( result == 0 )
    Port_LoadSRAM();   /* 读档失败（无存档）返回 -1，属正常，忽略 */

  return result;
}

/*-------------------------------------------------------------------*/
/*  启动 / 停止模拟线程                                               */
/*-------------------------------------------------------------------*/

extern "C" JNIEXPORT jint JNICALL
Java_com_infones_app_NativeBridge_nativeStart( JNIEnv *, jclass )
{
  std::lock_guard<std::mutex> lock( g_thread_mutex );

  /* 回收上一轮自然退出的线程 */
  if ( g_thread != nullptr )
  {
    if ( g_thread->joinable() )
      g_thread->join();
    delete g_thread;
    g_thread = nullptr;
  }

  Port_ClearStop();
  g_thread = new std::thread( emulation_thread_func );
  return ( g_thread != nullptr ) ? 0 : -1;
}

extern "C" JNIEXPORT void JNICALL
Java_com_infones_app_NativeBridge_nativeStop( JNIEnv *, jclass )
{
  {
    std::lock_guard<std::mutex> lock( g_thread_mutex );

    if ( g_thread != nullptr )
    {
      /* 请求停止 → HSync 在帧尾返回 -1 → Cycle 退出 → Menu 返回 -1 → Fin */
      Port_RequestStop();

      if ( g_thread->joinable() )
        g_thread->join();
      delete g_thread;
      g_thread = nullptr;
    }
  }

  /* Fin 已由 InfoNES_Main 尾部执行（pAPUDone + ReleaseRom），此处只落盘存档 */
  Port_SaveSRAM();
}

extern "C" JNIEXPORT jboolean JNICALL
Java_com_infones_app_NativeBridge_nativeIsRunning( JNIEnv *, jclass )
{
  std::lock_guard<std::mutex> lock( g_thread_mutex );
  return g_thread != nullptr ? JNI_TRUE : JNI_FALSE;
}

/*-------------------------------------------------------------------*/
/*  输入 / 设置                                                      */
/*-------------------------------------------------------------------*/

extern "C" JNIEXPORT void JNICALL
Java_com_infones_app_NativeBridge_nativeSetPadBit( JNIEnv *, jclass,
                                                   jint bit, jboolean down )
{
  if ( bit < 0 || bit > 7 )
    return;

  const uint32_t mask = 1u << bit;
  uint32_t bits = Port_GetPadBits();
  if ( down )
    bits |= mask;
  else
    bits &= ~mask;
  Port_SetPadBits( bits );
}

extern "C" JNIEXPORT void JNICALL
Java_com_infones_app_NativeBridge_nativeSetFrameSkip( JNIEnv *, jclass, jint frames )
{
  if ( frames < 0 )
    frames = 0;
  if ( frames > 9 )
    frames = 9;
  FrameSkip = (uint16_t)frames;
}

extern "C" JNIEXPORT void JNICALL
Java_com_infones_app_NativeBridge_nativeSetMute( JNIEnv *, jclass, jboolean mute )
{
  APU_Mute = mute ? 1 : 0;
}
