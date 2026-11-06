/*===================================================================*/
/*                                                                   */
/*                            Mapper 0  映射器 0                     */
/*                                                                   */
/*===================================================================*/

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 0  初始化映射器 0                              */
/*-------------------------------------------------------------------*/
void Map0_Init()
{
  /* Initialize Mapper  初始化映射器 */
  MapperInit = Map0_Init;

  /* Write to Mapper  写入映射器 */
  MapperWrite = Map0_Write;

  /* Write to SRAM  写入 SRAM */
  MapperSram = Map0_Sram;

  /* Write to APU  写入 APU */
  MapperApu = Map0_Apu;

  /* Read from APU  从 APU 读取 */
  MapperReadApu = Map0_ReadApu;

  /* Callback at VSync  场同步回调 */
  MapperVSync = Map0_VSync;

  /* Callback at HSync  行同步回调 */
  MapperHSync = Map0_HSync;

  /* Callback at PPU  PPU 回调 */
  MapperPPU = Map0_PPU;

  /* Callback at Rendering Screen ( 1:BG, 0:Sprite )  渲染画面回调（1：背景，0：精灵） */
  MapperRenderScreen = Map0_RenderScreen;

  /* Set SRAM Banks  设置 SRAM 银行 */
  SRAMBANK = SRAM;

  /* Set ROM Banks  设置 ROM 银行 */
  if ( NesHeader.rom_size > 1 )
  {
    ROMBANK0 = ROMPAGE( 0 );
    ROMBANK1 = ROMPAGE( 1 );
    ROMBANK2 = ROMPAGE( 2 );
    ROMBANK3 = ROMPAGE( 3 );
  }
  else if ( NesHeader.rom_size > 0 )
  {
    ROMBANK0 = ROMPAGE( 0 );
    ROMBANK1 = ROMPAGE( 1 );
    ROMBANK2 = ROMPAGE( 0 );
    ROMBANK3 = ROMPAGE( 1 );
  } else {
    ROMBANK0 = ROMPAGE( 0 );
    ROMBANK1 = ROMPAGE( 0 );
    ROMBANK2 = ROMPAGE( 0 );
    ROMBANK3 = ROMPAGE( 0 );
  }

  /* Set PPU Banks  设置 PPU 银行 */
  if ( NesHeader.v_rom_size > 0 )
  {
    for ( int page_number = 0; page_number < 8; ++page_number )
      PPUBANK[ page_number ] = VROMPAGE( page_number );
    InfoNES_SetupChr();
  }

  /* Set up wiring of the interrupt pin  设置中断引脚接线方式 */
  K6502_Set_Int_Wiring( 1, 1 ); 
}

/*-------------------------------------------------------------------*/
/*  Mapper 0 Write Function  映射器 0：写函数                        */
/*-------------------------------------------------------------------*/
void Map0_Write( uint16_t address, uint8_t byte_data )
{
/*
 *  Dummy Write to Mapper  对映射器的空写
 *
 */
}

/*-------------------------------------------------------------------*/
/*  Mapper 0 Write to SRAM Function  映射器 0：写 SRAM 函数          */
/*-------------------------------------------------------------------*/
void Map0_Sram( uint16_t address, uint8_t byte_data )
{
/*
 *  Dummy Write to Sram  对 SRAM 的空写
 *
 */
}

/*-------------------------------------------------------------------*/
/*  Mapper 0 Write to APU Function  映射器 0：写 APU 函数            */
/*-------------------------------------------------------------------*/
void Map0_Apu( uint16_t address, uint8_t byte_data )
{
/*
 *  Dummy Write to Apu  对 APU 的空写
 *
 */
}

/*-------------------------------------------------------------------*/
/*  Mapper 0 Read from APU Function  映射器 0：读 APU 函数           */
/*-------------------------------------------------------------------*/
uint8_t Map0_ReadApu( uint16_t address )
{
/*
 *  Dummy Read from Apu  APU 空读函数
 *
 */
  return ( address >> 8 );
}

/*-------------------------------------------------------------------*/
/*  Mapper 0 V-Sync Function  映射器 0：场同步函数                   */
/*-------------------------------------------------------------------*/
void Map0_VSync()
{
/*
 *  Dummy Callback at VSync  场同步空回调
 *
 */
}

/*-------------------------------------------------------------------*/
/*  Mapper 0 H-Sync Function  映射器 0：行同步函数                   */
/*-------------------------------------------------------------------*/
void Map0_HSync()
{
/*
 *  Dummy Callback at HSync  行同步空回调
 *
 */
#if 0 
  // Frame IRQ  帧 IRQ
  FrameStep += STEP_PER_SCANLINE;
  if ( FrameStep > STEP_PER_FRAME && FrameIRQ_Enable )
  {
    FrameStep %= STEP_PER_FRAME;
    IRQ_REQ;
    APU_Reg[ 0x4015 ] |= 0x40;
  }
#endif
}

/*-------------------------------------------------------------------*/
/*  Mapper 0 PPU Function  映射器 0：PPU 函数                        */
/*-------------------------------------------------------------------*/
void Map0_PPU( uint16_t address )
{
/*
 *  Dummy Callback at PPU  PPU 空回调
 *
 */
}

/*-------------------------------------------------------------------*/
/*  Mapper 0 Rendering Screen Function  映射器 0：渲染画面函数       */
/*-------------------------------------------------------------------*/
void Map0_RenderScreen( uint8_t op_mode )
{
/*
 *  Dummy Callback at Rendering Screen  渲染画面空回调
 *
 */
}
