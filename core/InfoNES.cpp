/*===================================================================*/
/*                                                                   */
/*  InfoNES.cpp : NES Emulator for Win32, Linux(x86), Linux(PS2)  InfoNES.cpp：用于 Win32/Linux(x86)/Linux(PS2) 的 NES 模拟器 */
/*                                                                   */
/*  2000/05/18  InfoNES Project ( based on nes_x )  InfoNES 项目（基于 nes_x） */
/*                                                                   */
/*===================================================================*/

/*-------------------------------------------------------------------
 * File List :  文件列表：
 *
 * [NES Hardware]  [NES 硬件]
 *   InfoNES.cpp
 *   InfoNES.h
 *   K6502_rw.h
 *
 * [Mapper function]  [映射器函数]
 *   InfoNES_Mapper.cpp
 *   InfoNES_Mapper.h
 *
 * [The function which depends on a system]  [与平台相关的函数]
 *   InfoNES_System_ooo.cpp (ooo is a system name. win, ...)  InfoNES_System_ooo.cpp（ooo 为平台名，如 win…）
 *   InfoNES_System.h
 *
 * [CPU]
 *   K6502.cpp
 *   K6502.h
 *
 * [Others]  [其他]
 *   InfoNES_Types.h
 *
 --------------------------------------------------------------------*/

/*-------------------------------------------------------------------*/
/*  Include files  包含文件                                          */
/*-------------------------------------------------------------------*/

#include "InfoNES.h"
#include "InfoNES_System.h"
#include "InfoNES_Mapper.h"
#include "InfoNES_pAPU.h"
#include "K6502.h"

/*-------------------------------------------------------------------*/
/*  NES resources  NES 资源                                          */
/*-------------------------------------------------------------------*/

/* RAM  随机存取存储器 */
uint8_t RAM[ RAM_SIZE ];

/* SRAM  存档 RAM（SRAM） */
uint8_t SRAM[ SRAM_SIZE ];

/* ROM  只读存储器 */
uint8_t *ROM;

/* SRAM BANK ( 8Kb )  SRAM 银行（8Kb） */
uint8_t *SRAMBANK;

/* ROM BANK ( 8Kb * 4 )  ROM 银行（8Kb * 4） */
uint8_t *ROMBANK0;
uint8_t *ROMBANK1;
uint8_t *ROMBANK2;
uint8_t *ROMBANK3;

/*-------------------------------------------------------------------*/
/*  PPU resources  PPU 资源                                          */
/*-------------------------------------------------------------------*/

/* PPU RAM  PPU 内存 */
uint8_t PPURAM[ PPURAM_SIZE ];

/* VROM  字符 ROM（VROM） */
uint8_t *VROM;

/* PPU BANK ( 1Kb * 16 )  PPU 银行（1Kb * 16） */
uint8_t *PPUBANK[ 16 ];

/* Sprite RAM  精灵 RAM */
uint8_t SPRRAM[ SPRRAM_SIZE ];

/* PPU Register  PPU 寄存器 */
uint8_t PPU_R0;
uint8_t PPU_R1;
uint8_t PPU_R2;
uint8_t PPU_R3;
uint8_t PPU_R7;

/* Vertical scroll value  垂直卷轴值 */
uint8_t PPU_Scr_V;
uint8_t PPU_Scr_V_Next;
uint8_t PPU_Scr_V_Byte;
uint8_t PPU_Scr_V_Byte_Next;
uint8_t PPU_Scr_V_Bit;
uint8_t PPU_Scr_V_Bit_Next;

/* Horizontal scroll value  水平卷轴值 */
uint8_t PPU_Scr_H;
uint8_t PPU_Scr_H_Next;
uint8_t PPU_Scr_H_Byte;
uint8_t PPU_Scr_H_Byte_Next;
uint8_t PPU_Scr_H_Bit;
uint8_t PPU_Scr_H_Bit_Next;

/* PPU Address  PPU 地址 */
uint16_t PPU_Addr;

/* PPU Address  PPU 地址 */
uint16_t PPU_Temp;

/* The increase value of the PPU Address  PPU 地址的增量 */
uint16_t PPU_Increment;

/* Current Scanline  当前扫描行 */
uint16_t PPU_Scanline;

/* Scanline Table  扫描行表 */
uint8_t PPU_ScanTable[ 263 ];

/* Name Table Bank  命名表银行 */
uint8_t PPU_NameTableBank;

/* BG Base Address  背景基地址 */
uint8_t *PPU_BG_Base;

/* Sprite Base Address  精灵基地址 */
uint8_t *PPU_SP_Base;

/* Sprite Height  精灵高度 */
uint16_t PPU_SP_Height;

/* Sprite #0 Scanline Hit Position  精灵 #0 扫描行命中位置 */
int SpriteJustHit;

/* VRAM Write Enable ( 0: Disable, 1: Enable )  VRAM 写使能（0：禁止，1：允许） */
uint8_t vram_write_enable;

/* PPU Address and Scroll Latch Flag*/
uint8_t PPU_Latch_Flag;

/* Up and Down Clipping Flag ( 0: non-clip, 1: clip )  上下裁剪标志（0：不裁剪，1：裁剪） */
uint8_t PPU_UpDown_Clip;

/* Frame IRQ ( 0: Disabled, 1: Enabled )*/
uint8_t FrameIRQ_Enable;
uint16_t FrameStep;

/*-------------------------------------------------------------------*/
/*  Display and Others resouces  画面与其他资源                      */
/*-------------------------------------------------------------------*/

/* Frame Skip  跳帧 */
uint16_t FrameSkip;
uint16_t FrameCnt;

/* Display Buffer  显示缓冲 */
#if 0
uint16_t DoubleFrame[ 2 ][ NES_DISP_WIDTH * NES_DISP_HEIGHT ];
uint16_t *WorkFrame;
uint16_t WorkFrameIdx;
#else
uint16_t WorkFrame[ NES_DISP_WIDTH * NES_DISP_HEIGHT ];
#endif

/* Character Buffer  字符缓冲 */
uint8_t ChrBuf[ 256 * 2 * 8 * 8 ];

/* Update flag for ChrBuf  ChrBuf 更新标志 */
uint8_t ChrBufUpdate;

/* Palette Table  调色板表 */
uint16_t PalTable[ 32 ];

/* Table for Mirroring  镜像查找表 */
uint8_t PPU_MirrorTable[][ 4 ] =
{
  { NAME_TABLE0, NAME_TABLE0, NAME_TABLE1, NAME_TABLE1 },
  { NAME_TABLE0, NAME_TABLE1, NAME_TABLE0, NAME_TABLE1 },
  { NAME_TABLE1, NAME_TABLE1, NAME_TABLE1, NAME_TABLE1 },
  { NAME_TABLE0, NAME_TABLE0, NAME_TABLE0, NAME_TABLE0 },
  { NAME_TABLE0, NAME_TABLE1, NAME_TABLE2, NAME_TABLE3 },
  { NAME_TABLE0, NAME_TABLE0, NAME_TABLE0, NAME_TABLE1 }
};

/*-------------------------------------------------------------------*/
/*  APU and Pad resources  APU 与手柄资源                            */
/*-------------------------------------------------------------------*/

/* APU Register  APU 寄存器 */
uint8_t APU_Reg[ 0x18 ];

/* APU Mute ( 0:OFF, 1:ON )  APU 静音（0：关，1：开） */
int APU_Mute = 0;

/* Pad data  手柄数据 */
uint32_t PAD1_Latch;
uint32_t PAD2_Latch;
uint32_t PAD_System;
uint32_t PAD1_Bit;
uint32_t PAD2_Bit;

/*-------------------------------------------------------------------*/
/*  Mapper Function  映射器函数                                      */
/*-------------------------------------------------------------------*/

/* Initialize Mapper  初始化映射器 */
void (*MapperInit)();
/* Write to Mapper  写入映射器 */
void (*MapperWrite)( uint16_t address, uint8_t byte_data );
/* Write to SRAM  写入 SRAM */
void (*MapperSram)( uint16_t address, uint8_t byte_data );
/* Write to Apu  写入 APU */
void (*MapperApu)( uint16_t address, uint8_t byte_data );
/* Read from Apu  从 APU 读取 */
uint8_t (*MapperReadApu)( uint16_t address );
/* Callback at VSync  场同步回调 */
void (*MapperVSync)();
/* Callback at HSync  行同步回调 */
void (*MapperHSync)();
/* Callback at PPU read/write  PPU 读写回调 */
void (*MapperPPU)( uint16_t address );
/* Callback at Rendering Screen 1:BG, 0:Sprite  渲染画面回调（1：背景，0：精灵） */
void (*MapperRenderScreen)( uint8_t op_mode );

/*-------------------------------------------------------------------*/
/*  ROM information  ROM 信息                                        */
/*-------------------------------------------------------------------*/

/* .nes File Header  .nes 文件头 */
struct NesHeader_tag NesHeader;

/* Mapper Number  映射器编号 */
uint8_t MapperNo;

/* Mirroring 0:Horizontal 1:Vertical  镜像 0：水平 1：垂直 */
uint8_t ROM_Mirroring;
/* It has SRAM  含 SRAM */
uint8_t ROM_SRAM;
/* It has Trainer  含训练器 */
uint8_t ROM_Trainer;
/* Four screen VRAM  四屏 VRAM */
uint8_t ROM_FourScr;

/*===================================================================*/
/*                                                                   */
/*                InfoNES_Init() : Initialize InfoNES  InfoNES_Init()：初始化 InfoNES */
/*                                                                   */
/*===================================================================*/
void InfoNES_Init()
{
/*
 *  Initialize InfoNES  初始化 InfoNES
 *
 *  Remarks  备注
 *    Initialize K6502 and Scanline Table.  初始化 K6502 与扫描行表
 */
  int slot_index;

  // Initialize 6502  初始化 6502
  K6502_Init();

  // Initialize Scanline Table  初始化扫描行表
  for ( slot_index = 0; slot_index < 263; ++slot_index )
  {
    if ( slot_index < SCAN_ON_SCREEN_START )
      PPU_ScanTable[ slot_index ] = SCAN_ON_SCREEN;
    else
    if ( slot_index < SCAN_BOTTOM_OFF_SCREEN_START )
      PPU_ScanTable[ slot_index ] = SCAN_ON_SCREEN;
    else
    if ( slot_index < SCAN_UNKNOWN_START )
      PPU_ScanTable[ slot_index ] = SCAN_ON_SCREEN;
    else
    if ( slot_index < SCAN_VBLANK_START )
      PPU_ScanTable[ slot_index ] = SCAN_UNKNOWN;
    else
      PPU_ScanTable[ slot_index ] = SCAN_VBLANK;
  }
}

/*===================================================================*/
/*                                                                   */
/*                InfoNES_Fin() : Completion treatment  InfoNES_Fin()：收尾处理 */
/*                                                                   */
/*===================================================================*/
void InfoNES_Fin()
{
/*
 *  Completion treatment  收尾处理
 *
 *  Remarks  备注
 *    Release resources  释放资源
 */
  // Finalize apu_state  反初始化 apu_state
  InfoNES_pAPUDone();

  // Release a memory for ROM  释放 ROM 内存
  InfoNES_ReleaseRom();
}

/*===================================================================*/
/*                                                                   */
/*                  InfoNES_Load() : Load a cassette  InfoNES_Load()：加载卡带 */
/*                                                                   */
/*===================================================================*/
int InfoNES_Load( const char *file_name )
{
/*
 *  Load a cassette  加载卡带
 *
 *  Parameters  参数
 *    const char *file_name            (Read)  （只读入参）
 *      File name of ROM image  ROM 镜像文件名
 *
 *  Return values  返回值
 *     0 : It was finished normally.  0：正常结束
 *    -1 : An error occurred.  -1：发生错误
 *
 *  Remarks  备注
 *    Read a ROM image in the memory.  将 ROM 镜像读入内存
 *    Reset InfoNES.  重置 InfoNES
 */

  // Release a memory for ROM  释放 ROM 内存
  InfoNES_ReleaseRom();

  // Read a ROM image in the memory  将 ROM 镜像读入内存
  if ( InfoNES_ReadRom( file_name ) < 0 )
    return -1;

  // Reset InfoNES  重置 InfoNES
  if ( InfoNES_Reset() < 0 )
    return -1;

  // Successful  成功
  return 0;
}

/*===================================================================*/
/*                                                                   */
/*                 InfoNES_Reset() : Reset InfoNES  InfoNES_Reset()：重置 InfoNES */
/*                                                                   */
/*===================================================================*/
int InfoNES_Reset()
{
/*
 *  Reset InfoNES  重置 InfoNES
 *
 *  Return values  返回值
 *     0 : Normally  0：正常
 *    -1 : Non support mapper  -1：不支持的映射器
 *
 *  Remarks  备注
 *    Initialize Resources, PPU and Mapper.  初始化资源、PPU 与映射器
 *    Reset CPU.  复位 CPU
 */

  int slot_index;

  /*-------------------------------------------------------------------*/
  /*  Get information on the cassette  获取卡带信息                    */
  /*-------------------------------------------------------------------*/

  // Get Mapper Number  获取映射器编号
  MapperNo = NesHeader.info1 >> 4;

  // Check bit counts of Mapper No.  检查映射器编号的位数
  for ( slot_index = 4; slot_index < 8 && NesHeader.reserve[ slot_index ] == 0; ++slot_index )
    ;

  if ( slot_index == 8 )
  {
    // Mapper Number is 8bits  映射器编号占 8 位
    MapperNo |= ( NesHeader.info2 & 0xf0 );
  }

  // Get information on the ROM  获取 ROM 信息
  ROM_Mirroring = NesHeader.info1 & 1;
  ROM_SRAM = NesHeader.info1 & 2;
  ROM_Trainer = NesHeader.info1 & 4;
  ROM_FourScr = NesHeader.info1 & 8;

  /*-------------------------------------------------------------------*/
  /*  Initialize resources  初始化资源                                 */
  /*-------------------------------------------------------------------*/

  // Clear RAM  清空 RAM
  InfoNES_MemorySet( RAM, 0, sizeof RAM );

  // Reset frame skip and frame count  重置跳帧与帧计数
  FrameSkip = 0;
  FrameCnt = 0;

#if 0
  // Reset work frame  重置工作帧
  WorkFrame = DoubleFrame[ 0 ];
  WorkFrameIdx = 0;
#endif

  // Reset update flag of ChrBuf  重置 ChrBuf 更新标志
  ChrBufUpdate = 0xff;

  // Reset palette table  重置调色板表
  InfoNES_MemorySet( PalTable, 0, sizeof PalTable );

  // Reset APU register  重置 APU 寄存器
  InfoNES_MemorySet( APU_Reg, 0, sizeof APU_Reg );

  // Reset joypad  重置手柄
  PAD1_Latch = PAD2_Latch = PAD_System = 0;
  PAD1_Bit = PAD2_Bit = 0;

  /*-------------------------------------------------------------------*/
  /*  Initialize PPU  初始化 PPU                                       */
  /*-------------------------------------------------------------------*/

  InfoNES_SetupPPU();

  /*-------------------------------------------------------------------*/
  /*  Initialize apu_state  初始化 apu_state                                */
  /*-------------------------------------------------------------------*/

  InfoNES_pAPUInit();

  /*-------------------------------------------------------------------*/
  /*  Initialize Mapper  初始化映射器                                  */
  /*-------------------------------------------------------------------*/

  // Get Mapper Table Index  获取映射器表索引
  for ( slot_index = 0; MapperTable[ slot_index ].mapper_no != -1; ++slot_index )
  {
    if ( MapperTable[ slot_index ].mapper_no == MapperNo )
      break;
  }

  if ( MapperTable[ slot_index ].mapper_no == -1 )
  {
    // Non support mapper  不支持的映射器
    InfoNES_MessageBox( "Mapper #%d is unsupported.\n", MapperNo );
    return -1;
  }

  // Set up a mapper initialization function  设置映射器初始化函数
  MapperTable[ slot_index ].mapper_init();

  /*-------------------------------------------------------------------*/
  /*  Reset CPU  复位 CPU                                              */
  /*-------------------------------------------------------------------*/

  K6502_Reset();

  // Successful  成功
  return 0;
}

/*===================================================================*/
/*                                                                   */
/*                InfoNES_SetupPPU() : Initialize PPU  InfoNES_SetupPPU()：初始化 PPU */
/*                                                                   */
/*===================================================================*/
void InfoNES_SetupPPU()
{
/*
 *  Initialize PPU  初始化 PPU
 *
 */
  int page_number;

  // Clear PPU and Sprite Memory  清空 PPU 与精灵内存
  InfoNES_MemorySet( PPURAM, 0, sizeof PPURAM );
  InfoNES_MemorySet( SPRRAM, 0, sizeof SPRRAM );

  // Reset PPU Register  重置 PPU 寄存器
  PPU_R0 = PPU_R1 = PPU_R2 = PPU_R3 = PPU_R7 = 0;

  // Reset latch flag  重置锁存标志
  PPU_Latch_Flag = 0;

  // Reset up and down clipping flag  重置上下裁剪标志
  PPU_UpDown_Clip = 0;

  FrameStep = 0;
  FrameIRQ_Enable = 0;

  // Reset Scroll values  重置卷轴值
  PPU_Scr_V = PPU_Scr_V_Next = PPU_Scr_V_Byte = PPU_Scr_V_Byte_Next = PPU_Scr_V_Bit = PPU_Scr_V_Bit_Next = 0;
  PPU_Scr_H = PPU_Scr_H_Next = PPU_Scr_H_Byte = PPU_Scr_H_Byte_Next = PPU_Scr_H_Bit = PPU_Scr_H_Bit_Next = 0;

  // Reset PPU address  重置 PPU 地址
  PPU_Addr = 0;
  PPU_Temp = 0;

  // Reset scanline  重置扫描行
  PPU_Scanline = 0;

  // Reset hit position of sprite #0  重置精灵 #0 命中位置
  SpriteJustHit = 0;

  // Reset information on PPU_R0  重置 PPU_R0 信息
  PPU_Increment = 1;
  PPU_NameTableBank = NAME_TABLE0;
  PPU_BG_Base = ChrBuf;
  PPU_SP_Base = ChrBuf + 256 * 64;
  PPU_SP_Height = 8;

  // Reset PPU banks  重置 PPU 银行
  for ( page_number = 0; page_number < 16; ++page_number )
    PPUBANK[ page_number ] = &PPURAM[ page_number * 0x400 ];

  /* Mirroring of Name Table  命名表镜像 */
  InfoNES_Mirroring( ROM_Mirroring );

  /* Reset VRAM Write Enable  重置 VRAM 写使能 */
  vram_write_enable = ( NesHeader.v_rom_size == 0 ) ? 1 : 0;
}

/*===================================================================*/
/*                                                                   */
/*       InfoNES_Mirroring() : Set up a Mirroring of Name Table  InfoNES_Mirroring()：设置命名表镜像 */
/*                                                                   */
/*===================================================================*/
void InfoNES_Mirroring( int entry_type )
{
/*
 *  Set up a Mirroring of Name Table  设置命名表镜像
 *
 *  Parameters  参数
 *    int entry_type          (Read)  （只读入参）
 *      Mirroring Type  镜像类型
 *        0 : Horizontal  0：水平
 *        1 : Vertical  1：垂直
 *        2 : One Screen 0x2400  2：单屏 0x2400
 *        3 : One Screen 0x2000  3：单屏 0x2000
 *        4 : Four Screen  4：四屏
 *        5 : Special for Mapper #233  5：映射器 #233 专用
 */

  PPUBANK[ NAME_TABLE0 ] = &PPURAM[ PPU_MirrorTable[ entry_type ][ 0 ] * 0x400 ];
  PPUBANK[ NAME_TABLE1 ] = &PPURAM[ PPU_MirrorTable[ entry_type ][ 1 ] * 0x400 ];
  PPUBANK[ NAME_TABLE2 ] = &PPURAM[ PPU_MirrorTable[ entry_type ][ 2 ] * 0x400 ];
  PPUBANK[ NAME_TABLE3 ] = &PPURAM[ PPU_MirrorTable[ entry_type ][ 3 ] * 0x400 ];
}

/*===================================================================*/
/*                                                                   */
/*              InfoNES_Main() : The main loop of InfoNES  InfoNES_Main()：InfoNES 主循环 */
/*                                                                   */
/*===================================================================*/
void InfoNES_Main()
{
/*
 *  The main loop of InfoNES  InfoNES 主循环
 *
 */

  // Initialize InfoNES  初始化 InfoNES
  InfoNES_Init();

  // Main loop  主循环
  while ( 1 )
  {
    /*-------------------------------------------------------------------*/
    /*  To the menu screen  返回菜单画面                                 */
    /*-------------------------------------------------------------------*/
    if ( InfoNES_Menu() == -1 )
      break;  // Quit  退出
    
    /*-------------------------------------------------------------------*/
    /*  Start a NES emulation  启动 NES 模拟                             */
    /*-------------------------------------------------------------------*/
    InfoNES_Cycle();
  }

  // Completion treatment  收尾处理
  InfoNES_Fin();
}

/*===================================================================*/
/*                                                                   */
/*              InfoNES_Cycle() : The loop of emulation  InfoNES_Cycle()：模拟循环 */
/*                                                                   */
/*===================================================================*/
void InfoNES_Cycle()
{
/*
 *  The loop of emulation  模拟循环
 *
 */

  // Set the PPU adress to the buffered value  将 PPU 地址设为缓冲值
  if ( ( PPU_R1 & R1_SHOW_SP ) || ( PPU_R1 & R1_SHOW_SCR ) )
		PPU_Addr = PPU_Temp;

  // Emulation loop  模拟循环
  for (;;)
  {    
    int step;

    // Set a flag if a scanning line is a hit in the sprite #0  扫描行命中精灵 #0 时置位标志
    if ( SpriteJustHit == PPU_Scanline &&
      PPU_ScanTable[ PPU_Scanline ] == SCAN_ON_SCREEN )
    {
      // # of Steps to execute before sprite #0 hit  精灵 #0 命中前需要执行的步数
      step = SPRRAM[ SPR_X ] * STEP_PER_SCANLINE / NES_DISP_WIDTH;

      // Execute instructions  执行指令
      K6502_Step( step );

      // Set a sprite hit flag  置精灵命中标志
      if ( ( PPU_R1 & R1_SHOW_SP ) && ( PPU_R1 & R1_SHOW_SCR ) )
        PPU_R2 |= R2_HIT_SP;

      // NMI is required if there is necessity  有需要时触发 NMI
      if ( ( PPU_R0 & R0_NMI_SP ) && ( PPU_R1 & R1_SHOW_SP ) )
        NMI_REQ;

      // Execute instructions  执行指令
      K6502_Step( STEP_PER_SCANLINE - step );
    }
    else
    {
      // Execute instructions  执行指令
      K6502_Step( STEP_PER_SCANLINE );
    }

    // Frame IRQ in H-Sync  行同步中的帧 IRQ
    FrameStep += STEP_PER_SCANLINE;
    if ( FrameStep > STEP_PER_FRAME && FrameIRQ_Enable )
    {
      FrameStep %= STEP_PER_FRAME;
      IRQ_REQ;
      APU_Reg[ 0x4015 ] |= 0x40;
    }

    // A mapper function in H-Sync  行同步中的映射器函数
    MapperHSync();
    
    // A function in H-Sync  行同步（H-Sync）中的函数
    if ( InfoNES_HSync() == -1 )
      return;  // To the menu screen  返回菜单画面

    // HSYNC Wait  行同步等待
    InfoNES_Wait();
  }
}

/*===================================================================*/
/*                                                                   */
/*              InfoNES_HSync() : A function in H-Sync  InfoNES_HSync()：行同步处理函数 */
/*                                                                   */
/*===================================================================*/
int InfoNES_HSync()
{
/*
 *  A function in H-Sync  行同步（H-Sync）中的函数
 *
 *  Return values  返回值
 *    0 : Normally  0：正常
 *   -1 : Exit an emulation  -1：退出模拟
 */

  /*-------------------------------------------------------------------*/
  /*  Render a scanline  渲染一条扫描行                                */
  /*-------------------------------------------------------------------*/
  if ( FrameCnt == 0 &&
       PPU_ScanTable[ PPU_Scanline ] == SCAN_ON_SCREEN )
  {
    InfoNES_DrawLine();
  }

  /*-------------------------------------------------------------------*/
  /*  Set new scroll values  设置新的卷轴值                            */
  /*-------------------------------------------------------------------*/
  PPU_Scr_V      = PPU_Scr_V_Next;
  PPU_Scr_V_Byte = PPU_Scr_V_Byte_Next;
  PPU_Scr_V_Bit  = PPU_Scr_V_Bit_Next;

  PPU_Scr_H      = PPU_Scr_H_Next;
  PPU_Scr_H_Byte = PPU_Scr_H_Byte_Next;
  PPU_Scr_H_Bit  = PPU_Scr_H_Bit_Next;

  /*-------------------------------------------------------------------*/
  /*  Next Scanline  下一条扫描行                                      */
  /*-------------------------------------------------------------------*/
  PPU_Scanline = ( PPU_Scanline == SCAN_VBLANK_END ) ? 0 : PPU_Scanline + 1;

  /*-------------------------------------------------------------------*/
  /*  Operation in the specific scanning line  特定扫描行上的处理      */
  /*-------------------------------------------------------------------*/
  switch ( PPU_Scanline )
  {
    case SCAN_TOP_OFF_SCREEN:
      // Reset a PPU status  复位 PPU 状态
      PPU_R2 = 0;

      // Set up a character data  设置字符数据
      if ( NesHeader.v_rom_size == 0 && FrameCnt == 0 )
        InfoNES_SetupChr();

      // Get position of sprite #0  获取精灵 #0 的位置
      InfoNES_GetSprHitY();
      break;

    case SCAN_UNKNOWN_START:
      if ( FrameCnt == 0 )
      {
        // Transfer the contents of work frame on the screen  将工作帧内容传输到屏幕
        InfoNES_LoadFrame();
        
#if 0
        // Switching of the double buffer  切换双缓冲
        WorkFrameIdx = 1 - WorkFrameIdx;
        WorkFrame = DoubleFrame[ WorkFrameIdx ];
#endif
      }
      break;

    case SCAN_VBLANK_START:
      // FrameCnt + 1  帧计数 +1
      FrameCnt = ( FrameCnt >= FrameSkip ) ? 0 : FrameCnt + 1;

      // Set a V-Blank flag  置场消隐标志
      PPU_R2 = R2_IN_VBLANK;

      // Reset latch flag  重置锁存标志
      PPU_Latch_Flag = 0;

      // apu_state Sound function in V-Sync  场同步中的声音处理（apu_state）
      if ( !APU_Mute )
        InfoNES_pAPUVsync();

      // A mapper function in V-Sync  场同步中的映射器函数
      MapperVSync();

      // Get the condition of the joypad  获取手柄状态
      InfoNES_PadState( &PAD1_Latch, &PAD2_Latch, &PAD_System );
      
      // NMI on V-Blank  场消隐时触发 NMI
      if ( PPU_R0 & R0_NMI_VB )
        NMI_REQ;

      // Exit an emulation if a QUIT button is pushed  按下退出按钮时结束模拟
      if ( PAD_PUSH( PAD_System, PAD_SYS_QUIT ) )
        return -1;  // Exit an emulation  退出模拟
      
      break;
  }

  // Successful  成功
  return 0;
}

/*===================================================================*/
/*                                                                   */
/*              InfoNES_DrawLine() : Render a scanline  InfoNES_DrawLine()：渲染一条扫描行 */
/*                                                                   */
/*===================================================================*/
void InfoNES_DrawLine()
{
/*
 *  Render a scanline  渲染一条扫描行
 *
 */

  /* 保护：扫描表将 0~242 行都标记为可见，但 WorkFrame 只有 240 行。
     没有此保护时第 240~242 行会越界写出数组末尾约 1.5KB。 */
  if ( PPU_Scanline >= NES_DISP_HEIGHT )
    return;

  int nX;
  int nY;
  int y4;
  int y_bit;
  uint16_t *pal_tbl;
  uint8_t *attr_base;
  uint16_t *pixel;
  int name_table_index;
  uint8_t *name_table_ptr;
  uint8_t *chr_data;
  uint8_t *sprram;
  int attr;
  int spr_cnt;
  int slot_index;
  int spr_data;
  uint8_t spr_col;
  uint8_t spr_buf[ NES_DISP_WIDTH + 7 ];

  /*-------------------------------------------------------------------*/
  /*  Render Background  渲染背景                                      */
  /*-------------------------------------------------------------------*/

  /* MMC5 VROM switch  MMC5 的 VROM 切换 */
  MapperRenderScreen( 1 );

  // Pointer to the render position  渲染位置指针
  pixel = &WorkFrame[ PPU_Scanline * NES_DISP_WIDTH ];

  // Clear a scanline if screen is off  屏幕关闭时清空该扫描行
  if ( !( PPU_R1 & R1_SHOW_SCR ) )
  {
    InfoNES_MemorySet( pixel, 0, NES_DISP_WIDTH << 1 );
  }
  else
  {
    name_table_index = PPU_NameTableBank;

    nY = PPU_Scr_V_Byte + ( PPU_Scanline >> 3 );

    y_bit = PPU_Scr_V_Bit + ( PPU_Scanline & 7 );

    if ( y_bit > 7 )
    {
      ++nY;
      y_bit &= 7;
    }
    y_bit <<= 3;

    if ( nY > 29 )
    {
      // Next NameTable (An up-down direction)  下一个命名表（上下方向）
      name_table_index ^= NAME_TABLE_V_MASK;
      nY -= 30;
    }

    nX = PPU_Scr_H_Byte;

    y4 = ( ( nY & 2 ) << 1 );

    /*-------------------------------------------------------------------*/
    /*  Rendering of the block of the left end  渲染左端区块             */
    /*-------------------------------------------------------------------*/

    name_table_ptr = PPUBANK[ name_table_index ] + nY * 32 + nX;
    chr_data = PPU_BG_Base + ( *name_table_ptr << 6 ) + y_bit;
    attr_base = PPUBANK[ name_table_index ] + 0x3c0 + ( nY / 4 ) * 8;
    pal_tbl =  &PalTable[ ( ( ( attr_base[ nX >> 2 ] >> ( ( nX & 2 ) + y4 ) ) & 3 ) << 2 ) ];

    for ( slot_index = PPU_Scr_H_Bit; slot_index < 8; ++slot_index )
    {
      *( pixel++ ) = pal_tbl[ chr_data[ slot_index ] ];
    }

    // Callback at PPU read/write  PPU 读写回调
    MapperPPU( PATTBL( chr_data ) );

    ++nX;
    ++name_table_ptr;

    /*-------------------------------------------------------------------*/
    /*  Rendering of the left table  渲染左侧命名表                      */
    /*-------------------------------------------------------------------*/

    for ( ; nX < 32; ++nX )
    {
      chr_data = PPU_BG_Base + ( *name_table_ptr << 6 ) + y_bit;
      pal_tbl = &PalTable[ ( ( ( attr_base[ nX >> 2 ] >> ( ( nX & 2 ) + y4 ) ) & 3 ) << 2 ) ];

      pixel[ 0 ] = pal_tbl[ chr_data[ 0 ] ]; 
      pixel[ 1 ] = pal_tbl[ chr_data[ 1 ] ];
      pixel[ 2 ] = pal_tbl[ chr_data[ 2 ] ];
      pixel[ 3 ] = pal_tbl[ chr_data[ 3 ] ];
      pixel[ 4 ] = pal_tbl[ chr_data[ 4 ] ];
      pixel[ 5 ] = pal_tbl[ chr_data[ 5 ] ];
      pixel[ 6 ] = pal_tbl[ chr_data[ 6 ] ];
      pixel[ 7 ] = pal_tbl[ chr_data[ 7 ] ];
      pixel += 8;

      // Callback at PPU read/write  PPU 读写回调
      MapperPPU( PATTBL( chr_data ) );

      ++name_table_ptr;
    }

    // Holizontal Mirror  水平镜像（原文拼写如此）
    name_table_index ^= NAME_TABLE_H_MASK;

    name_table_ptr = PPUBANK[ name_table_index ] + nY * 32;
    attr_base = PPUBANK[ name_table_index ] + 0x3c0 + ( nY / 4 ) * 8;

    /*-------------------------------------------------------------------*/
    /*  Rendering of the right table  渲染右侧命名表                     */
    /*-------------------------------------------------------------------*/

    for ( nX = 0; nX < PPU_Scr_H_Byte; ++nX )
    {
      chr_data = PPU_BG_Base + ( *name_table_ptr << 6 ) + y_bit;
      pal_tbl = &PalTable[ ( ( ( attr_base[ nX >> 2 ] >> ( ( nX & 2 ) + y4 ) ) & 3 ) << 2 ) ];

      pixel[ 0 ] = pal_tbl[ chr_data[ 0 ] ]; 
      pixel[ 1 ] = pal_tbl[ chr_data[ 1 ] ];
      pixel[ 2 ] = pal_tbl[ chr_data[ 2 ] ];
      pixel[ 3 ] = pal_tbl[ chr_data[ 3 ] ];
      pixel[ 4 ] = pal_tbl[ chr_data[ 4 ] ];
      pixel[ 5 ] = pal_tbl[ chr_data[ 5 ] ];
      pixel[ 6 ] = pal_tbl[ chr_data[ 6 ] ];
      pixel[ 7 ] = pal_tbl[ chr_data[ 7 ] ];
      pixel += 8;

      // Callback at PPU read/write  PPU 读写回调
      MapperPPU( PATTBL( chr_data ) );

      ++name_table_ptr;
    }

    /*-------------------------------------------------------------------*/
    /*  Rendering of the block of the right end  渲染右端区块            */
    /*-------------------------------------------------------------------*/

    chr_data = PPU_BG_Base + ( *name_table_ptr << 6 ) + y_bit;
    pal_tbl = &PalTable[ ( ( ( attr_base[ nX >> 2 ] >> ( ( nX & 2 ) + y4 ) ) & 3 ) << 2 ) ];
    for ( slot_index = 0; slot_index < PPU_Scr_H_Bit; ++slot_index )
    {
      pixel[ slot_index ] = pal_tbl[ chr_data[ slot_index ] ];
    }

    // Callback at PPU read/write  PPU 读写回调
    MapperPPU( PATTBL( chr_data ) );

    /*-------------------------------------------------------------------*/
    /*  Backgroud Clipping  背景裁剪（原文拼写如此）                     */
    /*-------------------------------------------------------------------*/
    if ( !( PPU_R1 & R1_CLIP_BG ) )
    {
      uint16_t *pixel_top;

      pixel_top = &WorkFrame[ PPU_Scanline * NES_DISP_WIDTH ];
      InfoNES_MemorySet( pixel_top, 0, 8 << 1 );
    }

    /*-------------------------------------------------------------------*/
    /*  Clear a scanline if up and down clipping flag is set  上下裁剪标志置位时清空该扫描行 */
    /*-------------------------------------------------------------------*/
    if ( PPU_UpDown_Clip && 
       ( SCAN_ON_SCREEN_START > PPU_Scanline || PPU_Scanline > SCAN_BOTTOM_OFF_SCREEN_START ) )
    {
      uint16_t *pixel_top;

      pixel_top = &WorkFrame[ PPU_Scanline * NES_DISP_WIDTH ];
      InfoNES_MemorySet( pixel_top, 0, NES_DISP_WIDTH << 1 );
    }  
  }

  /*-------------------------------------------------------------------*/
  /*  Render a sprite  渲染精灵                                        */
  /*-------------------------------------------------------------------*/

  /* MMC5 VROM switch  MMC5 的 VROM 切换 */
  MapperRenderScreen( 0 );

  if ( PPU_R1 & R1_SHOW_SP )
  {
    // Reset Scanline Sprite Count  重置扫描行精灵计数
    PPU_R2 &= ~R2_MAX_SP;

    // Reset sprite buffer  重置精灵缓冲
    InfoNES_MemorySet( spr_buf, 0, sizeof spr_buf );

    // Render a sprite to the sprite buffer  将精灵渲染到精灵缓冲区
    spr_cnt = 0;
    for ( sprram = SPRRAM + ( 63 << 2 ); sprram >= SPRRAM; sprram -= 4 )
    {
      nY = sprram[ SPR_Y ] + 1;
      if ( nY > PPU_Scanline || nY + PPU_SP_Height <= PPU_Scanline )
        continue;  // Next sprite  下一个精灵

     /*-------------------------------------------------------------------*/
     /*  A sprite in scanning line  扫描行上的精灵                        */
     /*-------------------------------------------------------------------*/

      // Holizontal Sprite Count +1  水平精灵计数 +1（原文拼写如此）
      ++spr_cnt;
      
      attr = sprram[ SPR_ATTR ];
      y_bit = PPU_Scanline - nY;
      y_bit = ( attr & SPR_ATTR_V_FLIP ) ? ( PPU_SP_Height - y_bit - 1 ) << 3 : y_bit << 3;

      if ( PPU_R0 & R0_SP_SIZE )
      {
        // Sprite size 8x16  精灵尺寸 8x16
        if ( sprram[ SPR_CHR ] & 1 )
        {
          chr_data = ChrBuf + 256 * 64 + ( ( sprram[ SPR_CHR ] & 0xfe ) << 6 ) + y_bit;
        }
        else
        {
          chr_data = ChrBuf + ( ( sprram[ SPR_CHR ] & 0xfe ) << 6 ) + y_bit;
        }
      }
      else
      {
        // Sprite size 8x8  精灵尺寸 8x8
        chr_data = PPU_SP_Base + ( sprram[ SPR_CHR ] << 6 ) + y_bit;
      }

      attr ^= SPR_ATTR_PRI;
      spr_col = ( attr & ( SPR_ATTR_COLOR | SPR_ATTR_PRI ) ) << 2;
      nX = sprram[ SPR_X ];

      if ( attr & SPR_ATTR_H_FLIP )
      {
        // Horizontal flip  水平翻转
        if ( chr_data[ 7 ] )
          spr_buf[ nX ]     = spr_col | chr_data[ 7 ];
        if ( chr_data[ 6 ] )
          spr_buf[ nX + 1 ] = spr_col | chr_data[ 6 ];
        if ( chr_data[ 5 ] )
          spr_buf[ nX + 2 ] = spr_col | chr_data[ 5 ];
        if ( chr_data[ 4 ] )
          spr_buf[ nX + 3 ] = spr_col | chr_data[ 4 ];
        if ( chr_data[ 3 ] )
          spr_buf[ nX + 4 ] = spr_col | chr_data[ 3 ];
        if ( chr_data[ 2 ] )
          spr_buf[ nX + 5 ] = spr_col | chr_data[ 2 ];
        if ( chr_data[ 1 ] )
          spr_buf[ nX + 6 ] = spr_col | chr_data[ 1 ];
        if ( chr_data[ 0 ] )
          spr_buf[ nX + 7 ] = spr_col | chr_data[ 0 ];
      }
      else
      {
        // Non flip  未翻转
        if ( chr_data[ 0 ] )
          spr_buf[ nX ]     = spr_col | chr_data[ 0 ];
        if ( chr_data[ 1 ] )
          spr_buf[ nX + 1 ] = spr_col | chr_data[ 1 ];
        if ( chr_data[ 2 ] )
          spr_buf[ nX + 2 ] = spr_col | chr_data[ 2 ];
        if ( chr_data[ 3 ] )
          spr_buf[ nX + 3 ] = spr_col | chr_data[ 3 ];
        if ( chr_data[ 4 ] )
          spr_buf[ nX + 4 ] = spr_col | chr_data[ 4 ];
        if ( chr_data[ 5 ] )
          spr_buf[ nX + 5 ] = spr_col | chr_data[ 5 ];
        if ( chr_data[ 6 ] )
          spr_buf[ nX + 6 ] = spr_col | chr_data[ 6 ];
        if ( chr_data[ 7 ] )
          spr_buf[ nX + 7 ] = spr_col | chr_data[ 7 ];
      }
    }

    // Rendering sprite  渲染精灵
    pixel -= ( NES_DISP_WIDTH - PPU_Scr_H_Bit );
    for ( nX = 0; nX < NES_DISP_WIDTH; ++nX )
    {
      spr_data = spr_buf[ nX ];
      if ( spr_data  && ( spr_data & 0x80 || pixel[ nX ] & 0x8000 ) )
      {
        pixel[ nX ] = PalTable[ ( spr_data & 0xf ) + 0x10 ];
      }
    }

    /*-------------------------------------------------------------------*/
    /*  Sprite Clipping  精灵裁剪                                        */
    /*-------------------------------------------------------------------*/
    if ( !( PPU_R1 & R1_CLIP_SP ) )
    {
      uint16_t *pixel_top;

      pixel_top = &WorkFrame[ PPU_Scanline * NES_DISP_WIDTH ];
      InfoNES_MemorySet( pixel_top, 0, 8 << 1 );
    }

    if ( spr_cnt >= 8 )
      PPU_R2 |= R2_MAX_SP;  // Set a flag of maximum sprites on scanline  置扫描行精灵数达到上限的标志
  }
}

/*===================================================================*/
/*                                                                   */
/* InfoNES_GetSprHitY() : Get a position of scanline hits sprite #0  InfoNES_GetSprHitY()：获取扫描行命中精灵 #0 的位置 */
/*                                                                   */
/*===================================================================*/
void InfoNES_GetSprHitY()
{
/*
 * Get a position of scanline hits sprite #0  获取扫描行命中精灵 #0 的位置
 *
 */

  int y_bit;
  uint8_t *chr_data;
  int off;

  if ( SPRRAM[ SPR_ATTR ] & SPR_ATTR_V_FLIP )
  {
    // Vertical flip  垂直翻转
    y_bit = ( PPU_SP_Height - 1 ) << 3;
    off = -8;
  }
  else
  {
    // Non flip  未翻转
    y_bit = 0;
    off = 8;   // ChrBuf holds one byte per pixel, so a row is 8 bytes  ChrBuf 每像素占 1 字节，因此一行 8 字节
  }

  if ( PPU_R0 & R0_SP_SIZE )
  {
    // Sprite size 8x16  精灵尺寸 8x16
    if ( SPRRAM[ SPR_CHR ] & 1 )
    {
      chr_data = ChrBuf + 256 * 64 + ( ( SPRRAM[ SPR_CHR ] & 0xfe ) << 6 ) + y_bit;
    }
    else
    {
      chr_data = ChrBuf + ( ( SPRRAM[ SPR_CHR ] & 0xfe ) << 6 ) + y_bit;
    } 
  }
  else
  {
    // Sprite size 8x8  精灵尺寸 8x8
    chr_data = PPU_SP_Base + ( SPRRAM[ SPR_CHR ] << 6 ) + y_bit;
  }

  if ( ( SPRRAM[ SPR_Y ] + 1 <= SCAN_UNKNOWN_START ) && ( SPRRAM[SPR_Y] > 0 ) )
	{
		for ( int line_number = 0; line_number < PPU_SP_Height; line_number++ )
		{
			if ( chr_data[ 0 ] | chr_data[ 1 ] | chr_data[ 2 ] | chr_data[ 3 ] |
			     chr_data[ 4 ] | chr_data[ 5 ] | chr_data[ 6 ] | chr_data[ 7 ] )
			{
        // Scanline hits sprite #0  扫描行命中精灵 #0
				SpriteJustHit = SPRRAM[SPR_Y] + 1 + line_number;
				line_number = SCAN_VBLANK_END;
			}
			chr_data += off;
		}
  } else {
    // Scanline didn't hit sprite #0  扫描行未命中精灵 #0
		SpriteJustHit = SCAN_UNKNOWN_START + 1;
  }
}

/*===================================================================*/
/*                                                                   */
/*            InfoNES_SetupChr() : Develop character data  InfoNES_SetupChr()：展开字符数据 */
/*                                                                   */
/*===================================================================*/
void InfoNES_SetupChr()
{
/*
 *  Develop character data  展开字符数据
 *
 */

  uint8_t *bg_data;
  uint8_t data1;
  uint8_t data2;
  int slot_index;
  int nY;
  int off;
  static uint8_t *prev_bank[ 8 ];
  int bank_select;

  for ( bank_select = 0; bank_select < 8; ++bank_select )
  {
    if ( prev_bank[ bank_select ] == PPUBANK[ bank_select ] && !( ( ChrBufUpdate >> bank_select ) & 1 ) )
      continue;  // Next bank  下一个银行

    /*-------------------------------------------------------------------*/
    /*  An address is different from the last time  地址与上次不同       */
    /*    or                                                             */
    /*  An update flag is being set  更新标志被置位                      */
    /*-------------------------------------------------------------------*/

    for ( slot_index = 0; slot_index < 64; ++slot_index )
    {
      off = ( bank_select << 12 ) + ( slot_index << 6 );

      for ( nY = 0; nY < 8; ++nY )
      {
        bg_data = PPUBANK[ bank_select ] + ( slot_index << 4 ) + nY;

        data1 = ( ( bg_data[ 0 ] >> 1 ) & 0x55 ) | ( bg_data[ 8 ] & 0xAA );
        data2 = ( bg_data[ 0 ] & 0x55 ) | ( ( bg_data[ 8 ] << 1 ) & 0xAA );

        ChrBuf[ off ]     = ( data1 >> 6 ) & 3;
        ChrBuf[ off + 1 ] = ( data2 >> 6 ) & 3;
        ChrBuf[ off + 2 ] = ( data1 >> 4 ) & 3;
        ChrBuf[ off + 3 ] = ( data2 >> 4 ) & 3;
        ChrBuf[ off + 4 ] = ( data1 >> 2 ) & 3;
        ChrBuf[ off + 5 ] = ( data2 >> 2 ) & 3;
        ChrBuf[ off + 6 ] = data1 & 3;
        ChrBuf[ off + 7 ] = data2 & 3;

        off += 8;
      }
    }
    // Keep this address  保留该地址
    prev_bank[ bank_select ] = PPUBANK[ bank_select ];
  }

  // Reset update flag  重置更新标志
  ChrBufUpdate = 0;
}
