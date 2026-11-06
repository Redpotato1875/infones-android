/*===================================================================*/
/*                                                                   */
/*  InfoNES_Sound_Android.cpp : AAudio 输出实现                       */
/*                                                                   */
/*  设计要点：                                                        */
/*  - 单生产者（模拟线程 Push）/ 单消费者（AAudio 回调线程）环形缓冲， */
/*    读写下标用原子变量，无需锁。                                    */
/*  - 缓冲满时丢弃最旧数据而非阻塞：帧率节拍由                        */
/*    InfoNES_Wait 的 60Hz 时钟负责，与音频解耦。                    */
/*  - 流断开（耳机插拔、路由切换）只记日志不自动重启，                */
/*    下次装载/复位时 Sound_Open 会重建。                             */
/*                                                                   */
/*===================================================================*/

#include <mutex>
#include <atomic>
#include <android/log.h>
#include <aaudio/AAudio.h>

#include "InfoNES_Sound_Android.h"

#define LOG_TAG "InfoNES"
#define LOGI(...) __android_log_print( ANDROID_LOG_INFO,  LOG_TAG, __VA_ARGS__ )
#define LOGE(...) __android_log_print( ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__ )

namespace
{

/* 环形缓冲容量：32768 帧 ≈ 0.74 秒 @44100（足够的抗抖动余量） */
const int kRingFrames = 32768;
int16_t g_ring[ kRingFrames ];

/* 单调递增的读写位置（对 kRingFrames 取模） */
std::atomic<uint64_t> g_read_pos( 0 );
std::atomic<uint64_t> g_write_pos( 0 );

std::mutex      g_stream_mutex;
AAudioStream   *g_stream = nullptr;
bool            g_stream_ok = false;

inline uint64_t ring_used()
{
  return g_write_pos.load( std::memory_order_acquire ) -
         g_read_pos.load( std::memory_order_acquire );
}

inline int16_t ring_pop()
{
  const uint64_t pos = g_read_pos.load( std::memory_order_relaxed );
  const int16_t v = g_ring[ pos % kRingFrames ];
  g_read_pos.store( pos + 1, std::memory_order_release );
  return v;
}

/* 满则先丢一个最旧采样（写指针前进），再写入 */
inline void ring_push( int16_t v )
{
  uint64_t w = g_write_pos.load( std::memory_order_relaxed );
  if ( w - g_read_pos.load( std::memory_order_acquire ) >= (uint64_t)kRingFrames )
    g_read_pos.store( g_read_pos.load( std::memory_order_relaxed ) + 1,
                      std::memory_order_relaxed );
  g_ring[ w % kRingFrames ] = v;
  g_write_pos.store( w + 1, std::memory_order_release );
}

void clear_ring()
{
  g_read_pos.store( 0, std::memory_order_relaxed );
  g_write_pos.store( 0, std::memory_order_relaxed );
}

/*-------------------------------------------------------------------*/
/*  AAudio 回调                                                       */
/*-------------------------------------------------------------------*/

aaudio_data_callback_result_t data_callback( AAudioStream *stream,
                                             void *user_data,
                                             void *audio_data,
                                             int32_t num_frames )
{
  (void)stream; (void)user_data;
  int16_t *out = (int16_t *)audio_data;

  for ( int32_t i = 0; i < num_frames; ++i )
  {
    if ( ring_used() > 0 )
      out[ i ] = ring_pop();
    else
      out[ i ] = 0;   /* 欠载时输出静音，而不是停流 */
  }
  return AAUDIO_CALLBACK_RESULT_CONTINUE;
}

void error_callback( AAudioStream *stream, void *user_data,
                     aaudio_result_t error )
{
  (void)stream; (void)user_data;
  LOGE( "AAudio 流错误 %d，等待下次装载重建", (int)error );
  g_stream_ok = false;
}

} /* namespace */

/*-------------------------------------------------------------------*/
/*  对外接口                                                          */
/*-------------------------------------------------------------------*/

int Sound_AA_Open( int samples_per_sync, int sample_rate )
{
  (void)samples_per_sync;

  std::lock_guard<std::mutex> lock( g_stream_mutex );

  if ( g_stream_ok )
    return 1;   /* 已打开（换卡/复位前的重复调用幂等处理） */

  AAudioStreamBuilder *builder = nullptr;
  aaudio_result_t result = AAudio_createStreamBuilder( &builder );
  if ( result != AAUDIO_OK || builder == nullptr )
  {
    LOGE( "创建 AAudioStreamBuilder 失败：%d", (int)result );
    return 0;
  }

  AAudioStreamBuilder_setDirection( builder, AAUDIO_DIRECTION_OUTPUT );
  AAudioStreamBuilder_setFormat( builder, AAUDIO_FORMAT_PCM_I16 );
  AAudioStreamBuilder_setChannelCount( builder, 1 );
  AAudioStreamBuilder_setSampleRate( builder, sample_rate );
  AAudioStreamBuilder_setSharingMode( builder, AAUDIO_SHARING_MODE_SHARED );
  AAudioStreamBuilder_setPerformanceMode( builder, AAUDIO_PERFORMANCE_MODE_LOW_LATENCY );
  AAudioStreamBuilder_setBufferCapacityInFrames( builder, kRingFrames < 4096 ? kRingFrames : 4096 );
  AAudioStreamBuilder_setDataCallback( builder, data_callback, nullptr );
  AAudioStreamBuilder_setErrorCallback( builder, error_callback, nullptr );

  result = AAudioStreamBuilder_openStream( builder, &g_stream );
  AAudioStreamBuilder_delete( builder );

  if ( result != AAUDIO_OK || g_stream == nullptr )
  {
    LOGE( "打开 AAudio 流失败：%d（无声音，模拟继续）", (int)result );
    g_stream = nullptr;
    return 0;
  }

  /* 回调模式下按突发大小设置内部缓冲，兼顾延迟与欠载 */
  {
    int32_t burst = AAudioStream_getFramesPerBurst( g_stream );
    if ( burst > 0 )
    {
      int32_t size = burst * 3;
      int32_t cap  = AAudioStream_getBufferCapacityInFrames( g_stream );
      if ( size > cap ) size = cap;
      AAudioStream_setBufferSizeInFrames( g_stream, size );
    }
  }

  result = AAudioStream_requestStart( g_stream );
  if ( result != AAUDIO_OK )
  {
    LOGE( "启动 AAudio 流失败：%d", (int)result );
    AAudioStream_close( g_stream );
    g_stream = nullptr;
    return 0;
  }

  clear_ring();
  g_stream_ok = true;
  LOGI( "AAudio 打开成功：44100Hz 单声道 I16，突发 %d 帧",
        (int)AAudioStream_getFramesPerBurst( g_stream ) );
  return 1;
}

void Sound_AA_Close( void )
{
  std::lock_guard<std::mutex> lock( g_stream_mutex );

  if ( g_stream != nullptr )
  {
    AAudioStream_requestStop( g_stream );
    AAudioStream_close( g_stream );
    g_stream = nullptr;
  }
  g_stream_ok = false;
}

void Sound_AA_Push( const int16_t *data, int samples )
{
  /* 流不在时不入队（静音），避免换卡间隙的旧数据残留 */
  if ( !g_stream_ok )
    return;

  for ( int i = 0; i < samples; ++i )
    ring_push( data[ i ] );
}
