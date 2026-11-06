/*===================================================================*/
/*                                                                   */
/*  InfoNES_Port_Android.h : Android 平台端口层控制接口                */
/*                                                                   */
/*  由 JNI 桥（infones_jni.cpp）调用，与模拟内核（core/）的 14 个     */
/*  契约回调（InfoNES_Port_Android.cpp 内实现）配套。                 */
/*                                                                   */
/*===================================================================*/

#ifndef InfoNES_PORT_ANDROID_H_INCLUDED
#define InfoNES_PORT_ANDROID_H_INCLUDED

#include <cstddef>
#include <cstdint>
#include <string>

struct ANativeWindow;   /* 前向声明，避免把 android/native_window.h 传播给无关文件 */

/*-------------------------------------------------------------------*/
/*  ROM / 存档                                                       */
/*-------------------------------------------------------------------*/

/* 注入 ROM 镜像字节（JNI 从 SAF 读入后交给端口层）。
   端口层接管 data（malloc 缓冲区）的所有权，下次注入或释放时 free。 */
void Port_SetRomImage( const uint8_t *data, size_t size );

/* 设置 ROM 显示名（用于生成 <名字>.srm 存档文件名，会做字符清洗） */
void Port_SetRomName( const std::string &name );

/* 设置应用私有文件目录（Activity::getFilesDir），.srm 存于其 saves/ 下 */
void Port_SetFilesDir( const std::string &dir );

/* 读 / 写电池存档（RLE 格式与 Win 版 InfoNES_App_Win.cpp 完全兼容） */
int  Port_LoadSRAM();   /* 0 成功或无需读；-1 失败（文件不存在也返回 -1，可忽略） */
int  Port_SaveSRAM();   /* 0 成功或无需写；-1 失败 */

/*-------------------------------------------------------------------*/
/*  显示                                                             */
/*-------------------------------------------------------------------*/

/* 接管 ANativeWindow_fromSurface 返回的引用，并设置 256x240 RGB565 几何。
   旧 surface（若存在）先被释放。 */
void Port_SetSurface( ANativeWindow *window );

/* 释放 surface；模拟线程此后 InfoNES_LoadFrame 直接丢弃帧。 */
void Port_ClearSurface();

/*-------------------------------------------------------------------*/
/*  线程生命周期                                                     */
/*-------------------------------------------------------------------*/

/* 请求停止：InfoNES_HSync 在帧尾返回 -1，内核自行退出并执行 InfoNES_Fin */
void Port_RequestStop();

/* 清除停止标志（nativeStart 创建新模拟线程前调用） */
void Port_ClearStop();

/* 查询停止标志 */
bool Port_StopRequested();

/*-------------------------------------------------------------------*/
/*  手柄输入（bit0=A, bit1=B, bit2=Select, bit3=Start,               */
/*  bit4=上, bit5=下, bit6=左, bit7=右；与 Win 版 InfoNES_PadState 一致） */
/*-------------------------------------------------------------------*/

uint32_t Port_GetPadBits();
void     Port_SetPadBits( uint32_t bits );

#endif /* !InfoNES_PORT_ANDROID_H_INCLUDED */
