/*===================================================================*/
/*                                                                   */
/*  InfoNES_Sound_Android.h : AAudio 输出（I16 单声道环形缓冲）        */
/*                                                                   */
/*  替代 Win 版 DirectSound 环形缓冲。AAudio 采用回调模式，            */
/*  回调线程从环形缓冲取样；模拟线程每帧 Push 735 个采样。             */
/*                                                                   */
/*===================================================================*/

#ifndef InfoNES_SOUND_ANDROID_H_INCLUDED
#define InfoNES_SOUND_ANDROID_H_INCLUDED

#include <cstdint>

/* 打开 AAudio 输出流（InfoNES_SoundOpen 时调用）。
   samples_per_sync：每帧采样数（735）；sample_rate：44100。
   返回 1 成功，0 失败（调用方视为无声音，继续模拟）。 */
int  Sound_AA_Open( int samples_per_sync, int sample_rate );

/* 关闭输出流（InfoNES_SoundClose 时调用） */
void Sound_AA_Close( void );

/* 混音后的 I16 采样入队（InfoNES_SoundOutput 时调用）。
   缓冲满时丢弃最旧数据，保证不阻塞模拟线程（节拍由 60Hz 时钟负责）。 */
void Sound_AA_Push( const int16_t *data, int samples );

#endif /* !InfoNES_SOUND_ANDROID_H_INCLUDED */
