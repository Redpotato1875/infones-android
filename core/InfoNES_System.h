/*===================================================================*/
/*                                                                   */
/*  InfoNES_System.h : The function which depends on a system  InfoNES_System.h：与平台相关的接口函数 */
/*                                                                   */
/*  2000/05/29  InfoNES Project ( based on nes_x )  InfoNES 项目（基于 nes_x） */
/*                                                                   */
/*===================================================================*/

#ifndef InfoNES_SYSTEM_H_INCLUDED
#define InfoNES_SYSTEM_H_INCLUDED

/*-------------------------------------------------------------------*/
/*  Include files  包含文件                                          */
/*-------------------------------------------------------------------*/

#include "InfoNES_Types.h"

/*-------------------------------------------------------------------*/
/*  Palette data  调色板数据                                         */
/*-------------------------------------------------------------------*/
extern uint16_t NesPalette[];

/*-------------------------------------------------------------------*/
/*  Function prototypes  函数原型                                    */
/*-------------------------------------------------------------------*/

/* Menu screen  菜单画面 */
int InfoNES_Menu();

/* Read ROM image file  读取 ROM 镜像文件 */
int InfoNES_ReadRom( const char *file_name );

/* Release a memory for ROM  释放 ROM 内存 */
void InfoNES_ReleaseRom();

/* Transfer the contents of work frame on the screen  将工作帧内容传输到屏幕 */
void InfoNES_LoadFrame();

/* Get a joypad state  获取手柄状态 */
void InfoNES_PadState( uint32_t *pad_state_1, uint32_t *pad_state_2, uint32_t *system_state );

/* memcpy */
void *InfoNES_MemoryCopy( void *dest, const void *src, int count );

/* memset */
void *InfoNES_MemorySet( void *dest, int c, int count );

/* Print debug message  打印调试信息 */
void InfoNES_DebugPrint( char *message_text );

/* Wait  等待 */
void InfoNES_Wait();

/* Sound Initialize  声音初始化 */
void InfoNES_SoundInit( void );

/* Sound Open  打开声音 */
int InfoNES_SoundOpen( int samples_per_sync, int sample_rate );

/* Sound Close  关闭声音 */
void InfoNES_SoundClose( void );

/* Sound Output 5 Waves - 2 Pulse, 1 Triangle, 1 Noise, 1 DPCM  声音输出 5 个波形——2 脉冲、1 三角、1 噪声、1 DPCM */
void InfoNES_SoundOutput(int samples, uint8_t *wave1, uint8_t *wave2, uint8_t *wave3, uint8_t *wave4, uint8_t *wave5);

/* Print system message  打印系统信息 */
void InfoNES_MessageBox( char *message_text, ... );

#endif /* !InfoNES_SYSTEM_H_INCLUDED */
