/*===================================================================*/
/*                                                                   */
/*  InfoNES.h : NES Emulator for Win32, Linux(x86), Linux(PS2)  InfoNES.h：用于 Win32/Linux(x86)/Linux(PS2) 的 NES 模拟器 */
/*                                                                   */
/*  2000/05/14  InfoNES Project ( based on nes_x )  InfoNES 项目（基于 nes_x） */
/*                                                                   */
/*===================================================================*/

#ifndef InfoNES_H_INCLUDED
#define InfoNES_H_INCLUDED

/*-------------------------------------------------------------------*/
/*  Include files  包含文件                                          */
/*-------------------------------------------------------------------*/

#include "InfoNES_Types.h"

/*-------------------------------------------------------------------*/
/*  NES resources  NES 资源                                          */
/*-------------------------------------------------------------------*/

#define RAM_SIZE     0x2000
#define SRAM_SIZE    0x2000
#define PPURAM_SIZE  0x4000
#define SPRRAM_SIZE  256

/* RAM  随机存取存储器 */
extern uint8_t RAM[];

/* SRAM  存档 RAM（SRAM） */
extern uint8_t SRAM[];

/* ROM  只读存储器 */
extern uint8_t *ROM;

/* SRAM BANK ( 8Kb )  SRAM 银行（8Kb） */
extern uint8_t *SRAMBANK;

/* ROM BANK ( 8Kb * 4 )  ROM 银行（8Kb * 4） */
extern uint8_t *ROMBANK0;
extern uint8_t *ROMBANK1;
extern uint8_t *ROMBANK2;
extern uint8_t *ROMBANK3;

/*-------------------------------------------------------------------*/
/*  PPU resources  PPU 资源                                          */
/*-------------------------------------------------------------------*/

/* PPU RAM  PPU 内存 */
extern uint8_t PPURAM[];

/* VROM  字符 ROM（VROM） */
extern uint8_t *VROM;

/* PPU BANK ( 1Kb * 16 )  PPU 银行（1Kb * 16） */
extern uint8_t *PPUBANK[];

#define NAME_TABLE0  8
#define NAME_TABLE1  9
#define NAME_TABLE2  10
#define NAME_TABLE3  11

#define NAME_TABLE_V_MASK 2
#define NAME_TABLE_H_MASK 1

/* Sprite RAM  精灵 RAM */
extern uint8_t SPRRAM[];

#define SPR_Y    0
#define SPR_CHR  1
#define SPR_ATTR 2
#define SPR_X    3
#define SPR_ATTR_COLOR  0x3
#define SPR_ATTR_V_FLIP 0x80
#define SPR_ATTR_H_FLIP 0x40
#define SPR_ATTR_PRI    0x20

/* PPU Register  PPU 寄存器 */
extern uint8_t PPU_R0;
extern uint8_t PPU_R1;
extern uint8_t PPU_R2;
extern uint8_t PPU_R3;
extern uint8_t PPU_R7;

extern uint8_t PPU_Scr_V;
extern uint8_t PPU_Scr_V_Next;
extern uint8_t PPU_Scr_V_Byte;
extern uint8_t PPU_Scr_V_Byte_Next;
extern uint8_t PPU_Scr_V_Bit;
extern uint8_t PPU_Scr_V_Bit_Next;

extern uint8_t PPU_Scr_H;
extern uint8_t PPU_Scr_H_Next;
extern uint8_t PPU_Scr_H_Byte;
extern uint8_t PPU_Scr_H_Byte_Next;
extern uint8_t PPU_Scr_H_Bit;
extern uint8_t PPU_Scr_H_Bit_Next;

extern uint8_t PPU_Latch_Flag;
extern uint16_t PPU_Addr;
extern uint16_t PPU_Temp;
extern uint16_t PPU_Increment;

extern uint8_t PPU_Latch_Flag;
extern uint8_t PPU_UpDown_Clip;

#define R0_NMI_VB      0x80
#define R0_NMI_SP      0x40
#define R0_SP_SIZE     0x20
#define R0_BG_ADDR     0x10
#define R0_SP_ADDR     0x08
#define R0_INC_ADDR    0x04
#define R0_NAME_ADDR   0x03

#define R1_BACKCOLOR   0xe0
#define R1_SHOW_SP     0x10
#define R1_SHOW_SCR    0x08
#define R1_CLIP_SP     0x04
#define R1_CLIP_BG     0x02
#define R1_MONOCHROME  0x01

#define R2_IN_VBLANK   0x80
#define R2_HIT_SP      0x40
#define R2_MAX_SP      0x20
#define R2_WRITE_FLAG  0x10

#define SCAN_TOP_OFF_SCREEN     0
#define SCAN_ON_SCREEN          1
#define SCAN_BOTTOM_OFF_SCREEN  2
#define SCAN_UNKNOWN            3
#define SCAN_VBLANK             4

#define SCAN_TOP_OFF_SCREEN_START       0 
#define SCAN_ON_SCREEN_START            8
#define SCAN_BOTTOM_OFF_SCREEN_START  232
#define SCAN_UNKNOWN_START            240
#define SCAN_VBLANK_START             243
#define SCAN_VBLANK_END               262

#define STEP_PER_SCANLINE             112
#define STEP_PER_FRAME                29828

/* Develop Scroll Registers  展开卷轴寄存器 */
#define InfoNES_SetupScr() \
{ \
  /* V-Scroll Register */ \
  PPU_Scr_V_Next = ( uint8_t )( PPU_Addr & 0x001f ); \
  PPU_Scr_V_Byte_Next = PPU_Scr_V_Next >> 3; \
  PPU_Scr_V_Bit_Next = PPU_Scr_V_Next & 0x07; \
  \
  /* H-Scroll Register */ \
  PPU_Scr_H_Next = ( uint8_t )( ( PPU_Addr & 0x03e0 ) >> 5 ); \
  PPU_Scr_H_Byte_Next = PPU_Scr_H_Next >> 3; \
  PPU_Scr_H_Bit_Next = PPU_Scr_H_Next & 0x07; \
}

/* Current Scanline  当前扫描行 */
extern uint16_t PPU_Scanline;

/* Scanline Table  扫描行表 */
extern uint8_t PPU_ScanTable[];

/* Name Table Bank  命名表银行 */
extern uint8_t PPU_NameTableBank;

/* BG Base Address  背景基地址 */
extern uint8_t *PPU_BG_Base;

/* Sprite Base Address  精灵基地址 */
extern uint8_t *PPU_SP_Base;

/* Sprite Height  精灵高度 */
extern uint16_t PPU_SP_Height;

/* NES display size  NES 显示尺寸 */
#define NES_DISP_WIDTH      256
#define NES_DISP_HEIGHT     240

/* VRAM Write Enable ( 0: Disable, 1: Enable )  VRAM 写使能（0：禁止，1：允许） */
extern uint8_t vram_write_enable;

/* Frame IRQ ( 0: Disabled, 1: Enabled )*/
extern uint8_t FrameIRQ_Enable;
extern uint16_t FrameStep;

/*-------------------------------------------------------------------*/
/*  Display and Others resouces  画面与其他资源                      */
/*-------------------------------------------------------------------*/

/* Frame Skip  跳帧 */
extern uint16_t FrameSkip;
extern uint16_t FrameCnt;
extern uint16_t FrameWait;

#if 0
extern uint16_t DoubleFrame[ 2 ][ NES_DISP_WIDTH * NES_DISP_HEIGHT ];
extern uint16_t *WorkFrame;
extern uint16_t WorkFrameIdx;
#else
extern uint16_t WorkFrame[ NES_DISP_WIDTH * NES_DISP_HEIGHT ];
#endif

extern uint8_t ChrBuf[];

extern uint8_t ChrBufUpdate;

extern uint16_t PalTable[];

/*-------------------------------------------------------------------*/
/*  APU and Pad resources  APU 与手柄资源                            */
/*-------------------------------------------------------------------*/

extern uint8_t APU_Reg[];
extern int APU_Mute;

extern uint32_t PAD1_Latch;
extern uint32_t PAD2_Latch;
extern uint32_t PAD_System;
extern uint32_t PAD1_Bit;
extern uint32_t PAD2_Bit;

#define PAD_SYS_QUIT   1
#define PAD_SYS_OK     2
#define PAD_SYS_CANCEL 4
#define PAD_SYS_UP     8
#define PAD_SYS_DOWN   0x10
#define PAD_SYS_LEFT   0x20
#define PAD_SYS_RIGHT  0x40

#define PAD_PUSH(a,b)  ( ( (a) & (b) ) != 0 )

/*-------------------------------------------------------------------*/
/*  Mapper Function  映射器函数                                      */
/*-------------------------------------------------------------------*/

/* Initialize Mapper  初始化映射器 */
extern void (*MapperInit)();
/* Write to Mapper  写入映射器 */
extern void (*MapperWrite)( uint16_t address, uint8_t byte_data );
/* Write to SRAM  写入 SRAM */
extern void (*MapperSram)( uint16_t address, uint8_t byte_data );
/* Write to APU  写入 APU */
extern void (*MapperApu)( uint16_t address, uint8_t byte_data );
/* Read from Apu  从 APU 读取 */
extern uint8_t (*MapperReadApu)( uint16_t address );
/* Callback at VSync  场同步回调 */
extern void (*MapperVSync)();
/* Callback at HSync  行同步回调 */
extern void (*MapperHSync)();
/* Callback at PPU read/write  PPU 读写回调 */
extern void (*MapperPPU)( uint16_t address );
/* Callback at Rendering Screen 1:BG, 0:Sprite  渲染画面回调（1：背景，0：精灵） */
extern void (*MapperRenderScreen)( uint8_t op_mode );

/*-------------------------------------------------------------------*/
/*  ROM information  ROM 信息                                        */
/*-------------------------------------------------------------------*/

/* .nes File Header  .nes 文件头 */
struct NesHeader_tag
{
  uint8_t id[ 4 ];
  uint8_t rom_size;
  uint8_t v_rom_size;
  uint8_t info1;
  uint8_t info2;
  uint8_t reserve[ 8 ];
};

/* .nes File Header  .nes 文件头 */
extern struct NesHeader_tag NesHeader;

/* Mapper No.  映射器编号 */
extern uint8_t MapperNo;

/* Other  其他 */
extern uint8_t ROM_Mirroring;
extern uint8_t ROM_SRAM;
extern uint8_t ROM_Trainer;
extern uint8_t ROM_FourScr;

/*-------------------------------------------------------------------*/
/*  Function prototypes  函数原型                                    */
/*-------------------------------------------------------------------*/

/* Initialize InfoNES  初始化 InfoNES */
void InfoNES_Init();

/* Completion treatment  收尾处理 */
void InfoNES_Fin();

/* Load a cassette  加载卡带 */
int InfoNES_Load( const char *file_name );

/* Reset InfoNES  重置 InfoNES */
int InfoNES_Reset();

/* Initialize PPU  初始化 PPU */
void InfoNES_SetupPPU();

/* Set up a Mirroring of Name Table  设置命名表镜像 */
void InfoNES_Mirroring( int entry_type );

/* The main loop of InfoNES  InfoNES 主循环 */
void InfoNES_Main();

/* The loop of emulation  模拟循环 */
void InfoNES_Cycle();

/* A function in H-Sync  行同步（H-Sync）中的函数 */
int InfoNES_HSync();

/* Render a scanline  渲染一条扫描行 */
void InfoNES_DrawLine();

/* Get a position of scanline hits sprite #0  获取扫描行命中精灵 #0 的位置 */
void InfoNES_GetSprHitY();

/* Develop character data  展开字符数据 */
void InfoNES_SetupChr();

#endif /* !InfoNES_H_INCLUDED */
