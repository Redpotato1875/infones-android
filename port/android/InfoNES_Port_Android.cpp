/*===================================================================*/
/*                                                                   */
/*  InfoNES_Port_Android.cpp : InfoNES_System.h 契约的 Android 实现    */
/*                                                                   */
/*  实现内核对外索取的 14 个回调 + NesPalette[64]。                    */
/*  关键设计（与 Win 版的差异）：                                     */
/*  - InfoNES_ReadRom 改为内存解析：ROM 字节由 JNI 从 SAF 读入后经     */
/*    Port_SetRomImage 注入，绕开 Android 分区存储对 fopen 的限制。   */
/*  - InfoNES_LoadFrame 用 555→565 查表转换写 ANativeWindow。         */
/*    注意：WorkFrame 的高位 0x8000 是精灵优先级标志（K6502_rw.h      */
/*    写入、InfoNES.cpp:1041 读取），因此内部必须保持 555+标志位的     */
/*    原生格式，输出时才转换，绝不能把 WorkFrame 直接 memcpy 到 565   */
/*    表面。                                                          */
/*  - InfoNES_Wait 用 60Hz 稳定时钟节拍，替代 Win 版的多媒体定时器 +   */
/*    Sleep(0) 忙等。                                                 */
/*  - SRAM 存档为 RLE 格式，与 Win 版 InfoNES_App_Win.cpp 的          */
/*    SaveSRAM/LoadSRAM 逐字节兼容（老 .srm 可直接用）。              */
/*                                                                   */
/*===================================================================*/

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cstdarg>
#include <atomic>
#include <mutex>
#include <thread>
#include <chrono>
#include <string>
#include <sys/stat.h>

#include <android/log.h>
#include <android/native_window.h>

#include "InfoNES.h"
#include "InfoNES_System.h"
#include "InfoNES_Port_Android.h"
#include "InfoNES_Sound_Android.h"

#define LOG_TAG "InfoNES"
#define LOGI(...) __android_log_print( ANDROID_LOG_INFO,  LOG_TAG, __VA_ARGS__ )
#define LOGW(...) __android_log_print( ANDROID_LOG_WARN,  LOG_TAG, __VA_ARGS__ )

/*===================================================================*/
/*  调色板数据（RGB555，与 Win 版一致；0x8000 位保留作优先级标志）      */
/*===================================================================*/

uint16_t NesPalette[ 64 ] =
{
  0x39ce, 0x1071, 0x0015, 0x2013, 0x440e, 0x5402, 0x5000, 0x3c20,
  0x20a0, 0x0100, 0x0140, 0x00e2, 0x0ceb, 0x0000, 0x0000, 0x0000,
  0x5ef7, 0x01dd, 0x10fd, 0x401e, 0x5c17, 0x700b, 0x6ca0, 0x6521,
  0x45c0, 0x0240, 0x02a0, 0x0247, 0x0211, 0x0000, 0x0000, 0x0000,
  0x7fff, 0x1eff, 0x2e5f, 0x223f, 0x79ff, 0x7dd6, 0x7dcc, 0x7e67,
  0x7ae7, 0x4342, 0x2769, 0x2ff3, 0x03bb, 0x0000, 0x0000, 0x0000,
  0x7fff, 0x579f, 0x635f, 0x6b3f, 0x7f1f, 0x7f1b, 0x7ef6, 0x7f75,
  0x7f94, 0x73f4, 0x57d7, 0x5bf9, 0x4ffe, 0x0000, 0x0000, 0x0000
};

namespace
{

/*-------------------------------------------------------------------*/
/*  555 → 565 查表（屏蔽 0x8000 优先级标志位后索引）                  */
/*-------------------------------------------------------------------*/

uint16_t g_lut[ 32768 ];
const bool g_lut_init = []()
{
  for ( int i = 0; i < 32768; ++i )
  {
    const uint32_t r = ( i >> 10 ) & 0x1f;
    const uint32_t g = ( i >> 5 )  & 0x1f;
    const uint32_t b = i & 0x1f;
    g_lut[ i ] = (uint16_t)( ( r << 11 ) | ( ( ( g << 1 ) | ( g >> 4 ) ) << 5 ) | b );
  }
  return true;
}();

/*-------------------------------------------------------------------*/
/*  端口层状态                                                        */
/*-------------------------------------------------------------------*/

std::mutex  g_surface_mutex;
ANativeWindow *g_window = nullptr;

const uint8_t *g_rom_image = nullptr;   /* JNI 注入，端口层持有所有权 */
size_t        g_rom_size = 0;

std::string g_rom_name;                 /* ROM 显示名（生成 .srm 用） */
std::string g_save_dir;                 /* filesDir/saves */

std::atomic<bool>     g_stop_requested( false );
std::atomic<uint32_t> g_pad_bits( 0 );

int g_sram_save_flag = 0;

std::string sanitize_name( const std::string &name )
{
  std::string out;
  for ( size_t i = 0; i < name.size() && out.size() < 64; ++i )
  {
    const char c = name[ i ];
    if ( ( c >= 'a' && c <= 'z' ) || ( c >= 'A' && c <= 'Z' ) ||
         ( c >= '0' && c <= '9' ) || c == '-' || c == '_' )
      out += c;
    else
      out += '_';
  }
  if ( out.empty() )
    out = "rom";
  return out;
}

std::string sram_path()
{
  return g_save_dir + "/" + sanitize_name( g_rom_name ) + ".srm";
}

} /* namespace */

/*===================================================================*/
/*  端口层控制接口（JNI 调用）                                        */
/*===================================================================*/

void Port_SetRomImage( const uint8_t *data, size_t size )
{
  free( const_cast<uint8_t *>( g_rom_image ) );
  g_rom_image = data;
  g_rom_size = size;
}

void Port_SetRomName( const std::string &name )
{
  g_rom_name = name;
}

void Port_SetFilesDir( const std::string &dir )
{
  /* filesDir 直接作为存档根目录，无需再拼 saves/ */
  g_save_dir = dir;
#if defined( _WIN32 )
  mkdir( g_save_dir.c_str() );            /* Win 下的本地检查用单参形态 */
#else
  mkdir( g_save_dir.c_str(), 0770 );      /* Android/bionic POSIX 形态 */
#endif
  /* 已存在时 mkdir 返回 EEXIST，忽略即可 */
}

void Port_SetSurface( ANativeWindow *window )
{
  std::lock_guard<std::mutex> lock( g_surface_mutex );

  if ( g_window != nullptr )
    ANativeWindow_release( g_window );
  g_window = window;
  if ( g_window != nullptr )
    ANativeWindow_setBuffersGeometry( g_window, NES_DISP_WIDTH, NES_DISP_HEIGHT,
                                      WINDOW_FORMAT_RGB_565 );
}

void Port_ClearSurface()
{
  std::lock_guard<std::mutex> lock( g_surface_mutex );
  if ( g_window != nullptr )
  {
    ANativeWindow_release( g_window );
    g_window = nullptr;
  }
}

void Port_RequestStop() { g_stop_requested.store( true ); }
void Port_ClearStop()   { g_stop_requested.store( false ); }
bool Port_StopRequested() { return g_stop_requested.load(); }

uint32_t Port_GetPadBits() { return g_pad_bits.load(); }
void Port_SetPadBits( uint32_t bits ) { g_pad_bits.store( bits ); }

/*===================================================================*/
/*  存档（RLE，与 Win 版兼容）                                        */
/*===================================================================*/

int Port_LoadSRAM()
{
  g_sram_save_flag = 0;

  /* ROM 不带电池存档时无需读档 */
  if ( !ROM_SRAM )
    return 0;
  g_sram_save_flag = 1;

  if ( g_save_dir.empty() )
    return -1;

  FILE *fp = fopen( sram_path().c_str(), "rb" );
  if ( fp == nullptr )
    return -1;   /* 无存档属正常情况，调用方可忽略返回值 */

  uint8_t src[ SRAM_SIZE ];
  const size_t got = fread( src, 1, sizeof src, fp );
  fclose( fp );

  if ( got < 1 )
    return -1;

  size_t read_pos = 0;
  size_t write_pos = 0;
  const uint8_t tag = src[ read_pos++ ];

  /* 带边界检查的解码：坏档不会越界（Win 版没有这层保护） */
  while ( write_pos < SRAM_SIZE && read_pos < got )
  {
    const uint8_t data = src[ read_pos++ ];

    if ( data == tag )
    {
      if ( read_pos + 2 > got )
        break;   /* 截断的逃逸三元组 */
      const uint8_t value = src[ read_pos++ ];
      const int run_len = (int)src[ read_pos++ ] + 1;
      for ( int i = 0; i < run_len && write_pos < SRAM_SIZE; ++i )
        SRAM[ write_pos++ ] = value;
    }
    else
    {
      SRAM[ write_pos++ ] = data;
    }
  }

  return 0;
}

int Port_SaveSRAM()
{
  if ( !g_sram_save_flag )
    return 0;
  if ( g_save_dir.empty() )
    return -1;

#if defined( _WIN32 )
  mkdir( g_save_dir.c_str() );
#else
  mkdir( g_save_dir.c_str(), 0770 );
#endif

  /* 选出现次数最少的字节值作逃逸标记 */
  int used[ 256 ] = { 0 };
  for ( int i = 0; i < SRAM_SIZE; ++i )
    ++used[ SRAM[ i ] ];
  int tag = 0;
  for ( int i = 1; i < 256; ++i )
    if ( used[ i ] < used[ tag ] )
      tag = i;

  uint8_t dst[ SRAM_SIZE ];
  int enc_len = 0;
  dst[ enc_len++ ] = (uint8_t)tag;

  int run_len = 1;
  uint8_t prev = SRAM[ 0 ];
  int pos = 1;

  while ( pos < SRAM_SIZE && enc_len < (int)sizeof dst - 300 )
  {
    const uint8_t cur = SRAM[ pos++ ];

    if ( cur == prev && run_len < 256 )
    {
      ++run_len;
      continue;
    }

    if ( run_len >= 4 || prev == tag )
    {
      if ( enc_len + 3 > (int)sizeof dst )
        break;
      dst[ enc_len++ ] = (uint8_t)tag;
      dst[ enc_len++ ] = prev;
      dst[ enc_len++ ] = (uint8_t)( run_len - 1 );
    }
    else
    {
      for ( int i = 0; i < run_len && enc_len < (int)sizeof dst; ++i )
        dst[ enc_len++ ] = prev;
    }

    prev = cur;
    run_len = 1;
  }

  if ( run_len >= 4 || prev == tag )
  {
    if ( enc_len + 3 <= (int)sizeof dst )
    {
      dst[ enc_len++ ] = (uint8_t)tag;
      dst[ enc_len++ ] = prev;
      dst[ enc_len++ ] = (uint8_t)( run_len - 1 );
    }
  }
  else
  {
    for ( int i = 0; i < run_len && enc_len < (int)sizeof dst; ++i )
      dst[ enc_len++ ] = prev;
  }

  FILE *fp = fopen( sram_path().c_str(), "wb" );
  if ( fp == nullptr )
    return -1;
  const size_t written = fwrite( dst, 1, enc_len, fp );
  fclose( fp );

  return ( written == (size_t)enc_len ) ? 0 : -1;
}

/*===================================================================*/
/*  InfoNES_System.h 的 14 个契约实现                                 */
/*===================================================================*/

/*-------------------------------------------------------------------*/
/*  InfoNES_Menu() : 菜单（此处兼作"停止是否被请求"的询问点）           */
/*-------------------------------------------------------------------*/

int InfoNES_Menu()
{
  return Port_StopRequested() ? -1 : 0;
}

/*-------------------------------------------------------------------*/
/*  InfoNES_ReadRom() : 从内存镜像解析 ROM                            */
/*-------------------------------------------------------------------*/

int InfoNES_ReadRom( const char *file_name )
{
  (void)file_name;   /* ROM 字节由 Port_SetRomImage 注入，文件名仅作标识 */

  if ( g_rom_image == nullptr || g_rom_size < sizeof NesHeader )
    return -1;

  /* 读 ROM 文件头 */
  memcpy( &NesHeader, g_rom_image, sizeof NesHeader );
  if ( memcmp( NesHeader.id, "NES\x1a", 4 ) != 0 )
    return -1;   /* 不是 .nes 镜像 */

  /* 清 SRAM */
  memset( SRAM, 0, SRAM_SIZE );

  size_t offset = sizeof NesHeader;

  /* 有 trainer 时读到 0x7000-0x71ff（SRAM 偏移 0x1000） */
  if ( NesHeader.info1 & 4 )
  {
    if ( offset + 512 > g_rom_size )
      return -1;
    memcpy( &SRAM[ 0x1000 ], g_rom_image + offset, 512 );
    offset += 512;
  }

  /* PRG ROM */
  const size_t prg_size = (size_t)NesHeader.rom_size * 0x4000;
  if ( offset + prg_size > g_rom_size )
    return -1;
  uint8_t *rom = (uint8_t *)malloc( prg_size > 0 ? prg_size : 1 );
  if ( rom == nullptr )
    return -1;
  memcpy( rom, g_rom_image + offset, prg_size );
  offset += prg_size;

  /* CHR ROM（可为 0，此时用 ChrBuf 当 CHR-RAM） */
  uint8_t *vrom = nullptr;
  if ( NesHeader.v_rom_size > 0 )
  {
    const size_t chr_size = (size_t)NesHeader.v_rom_size * 0x2000;
    if ( offset + chr_size > g_rom_size )
    {
      free( rom );
      return -1;
    }
    vrom = (uint8_t *)malloc( chr_size );
    if ( vrom == nullptr )
    {
      free( rom );
      return -1;
    }
    memcpy( vrom, g_rom_image + offset, chr_size );
  }

  ROM = rom;
  VROM = vrom;
  return 0;
}

/*-------------------------------------------------------------------*/
/*  InfoNES_ReleaseRom() : 释放 ROM 银行缓冲                          */
/*-------------------------------------------------------------------*/

void InfoNES_ReleaseRom()
{
  if ( ROM )
  {
    free( ROM );
    ROM = nullptr;
  }
  if ( VROM )
  {
    free( VROM );
    VROM = nullptr;
  }
}

/*-------------------------------------------------------------------*/
/*  InfoNES_LoadFrame() : WorkFrame → ANativeWindow                  */
/*-------------------------------------------------------------------*/

void InfoNES_LoadFrame()
{
  std::lock_guard<std::mutex> lock( g_surface_mutex );
  if ( g_window == nullptr )
    return;

  ANativeWindow_Buffer buf;
  ARect dirty = { 0, 0, NES_DISP_WIDTH, NES_DISP_HEIGHT };
  if ( ANativeWindow_lock( g_window, &buf, &dirty ) != 0 )
    return;

  if ( buf.width >= NES_DISP_WIDTH && buf.height >= NES_DISP_HEIGHT )
  {
    const int bpp = ( buf.format == WINDOW_FORMAT_RGB_565 ) ? 2
                  : ( buf.format == WINDOW_FORMAT_RGBA_8888 ||
                      buf.format == WINDOW_FORMAT_RGBX_8888 ) ? 4
                  : 0;

    if ( bpp != 0 )
    {
      for ( int y = 0; y < NES_DISP_HEIGHT; ++y )
      {
        const uint16_t *src = &WorkFrame[ y * NES_DISP_WIDTH ];
        uint8_t *row = (uint8_t *)buf.bits + (size_t)y * (size_t)buf.stride * (size_t)bpp;
        uint16_t *dst16 = (uint16_t *)row;
        uint32_t *dst32 = (uint32_t *)row;

        for ( int x = 0; x < NES_DISP_WIDTH; ++x )
        {
          /* 屏蔽精灵优先级标志位后做 555→565 转换 */
          const uint16_t v = g_lut[ src[ x ] & 0x7fff ];

          if ( bpp == 2 )
            dst16[ x ] = v;
          else
          {
            const uint32_t r = ( (uint32_t)( v >> 11 ) & 0x1f ) * 255u / 31u;
            const uint32_t g = ( (uint32_t)( v >> 5 )  & 0x3f ) * 255u / 63u;
            const uint32_t b = ( (uint32_t)( v )       & 0x1f ) * 255u / 31u;
            dst32[ x ] = 0xff000000u | ( r << 16 ) | ( g << 8 ) | b;
          }
        }
      }
    }
  }

  ANativeWindow_unlockAndPost( g_window );
}

/*-------------------------------------------------------------------*/
/*  InfoNES_PadState() : 读取手柄状态                                 */
/*-------------------------------------------------------------------*/

void InfoNES_PadState( uint32_t *pad_state_1, uint32_t *pad_state_2,
                       uint32_t *system_state )
{
  const uint32_t bits = Port_GetPadBits();

  /* 与 Win 版一致：低 8 位复制到高 8 位 */
  *pad_state_1 = bits | ( bits << 8 );
  *pad_state_2 = 0;
  *system_state = Port_StopRequested() ? PAD_SYS_QUIT : 0;
}

/*-------------------------------------------------------------------*/
/*  内存操作：直接用 libc（契约允许，内核只要求语义等价）             */
/*-------------------------------------------------------------------*/

void *InfoNES_MemoryCopy( void *dest, const void *src, int count )
{
  return memcpy( dest, src, (size_t)count );
}

void *InfoNES_MemorySet( void *dest, int c, int count )
{
  return memset( dest, c, (size_t)count );
}

/*-------------------------------------------------------------------*/
/*  日志                                                             */
/*-------------------------------------------------------------------*/

void InfoNES_DebugPrint( char *message_text )
{
  LOGI( "%s", message_text );
}

void InfoNES_MessageBox( char *message_text, ... )
{
  va_list args;
  va_start( args, message_text );
  __android_log_vprint( ANDROID_LOG_WARN, LOG_TAG, message_text, args );
  va_end( args );
}

/*-------------------------------------------------------------------*/
/*  声音：5 路 uint8 波 → I16 混音入 AAudio 环形缓冲                  */
/*-------------------------------------------------------------------*/

void InfoNES_SoundInit( void )
{
  /* 打开放在 InfoNES_SoundOpen（pAPUInit 时由内核调用） */
}

int InfoNES_SoundOpen( int samples_per_sync, int sample_rate )
{
  return Sound_AA_Open( samples_per_sync, sample_rate ) ? 1 : 0;
}

void InfoNES_SoundClose( void )
{
  Sound_AA_Close();
}

void InfoNES_SoundOutput( int samples, uint8_t *wave1, uint8_t *wave2,
                          uint8_t *wave3, uint8_t *wave4, uint8_t *wave5 )
{
  int16_t mix[ 1024 ];

  while ( samples > 0 )
  {
    const int chunk = ( samples > 1024 ) ? 1024 : samples;

    for ( int i = 0; i < chunk; ++i )
    {
      /* 5 路等权平均；输入以 128 为中心的无符号 8 位 */
      const uint32_t avg = ( (uint32_t)wave1[ i ] + wave2[ i ] + wave3[ i ] +
                             wave4[ i ] + wave5[ i ] ) / 5u;
      mix[ i ] = (int16_t)( ( (int)avg - 128 ) << 8 );
    }

    Sound_AA_Push( mix, chunk );

    samples -= chunk;
    wave1 += chunk; wave2 += chunk; wave3 += chunk; wave4 += chunk; wave5 += chunk;
  }
}

/*-------------------------------------------------------------------*/
/*  InfoNES_Wait() : 60Hz 帧节拍                                     */
/*  每扫描行调用一次，263 行/帧累计到帧边界时对齐一次时钟。            */
/*  落后过多（切后台回来）时重整节拍而不是疯狂追帧。                  */
/*-------------------------------------------------------------------*/

void InfoNES_Wait()
{
  static int lines = 0;

  if ( ++lines < 263 )
    return;
  lines = 0;

  using clock = std::chrono::steady_clock;
  static clock::time_point next_frame( clock::duration( 0 ) );
  const clock::duration period( 16666667 );   /* 60.0Hz，与 735 采样/帧 @44100 对应 */

  next_frame += period;
  const clock::time_point now = clock::now();
  if ( next_frame < now )
    next_frame = now + period;   /* 掉帧过多：重新对齐 */

  std::this_thread::sleep_until( next_frame );
}
