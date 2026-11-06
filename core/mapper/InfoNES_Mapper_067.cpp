/*===================================================================*/
/*                                                                   */
/*               Mapper 67 (Sunsoft Mapper #3)  映射器 67（Sunsoft Mapper #3） */
/*                                                                   */
/*===================================================================*/

uint8_t Map67_IRQ_Enable;
uint8_t Map67_IRQ_Cnt;
uint8_t Map67_IRQ_Latch;

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 67  初始化映射器 67                            */
/*-------------------------------------------------------------------*/
void Map67_Init()
{
  /* Initialize Mapper  初始化映射器 */
  MapperInit = Map67_Init;

  /* Write to Mapper  写入映射器 */
  MapperWrite = Map67_Write;

  /* Write to SRAM  写入 SRAM */
  MapperSram = Map0_Sram;

  /* Write to APU  写入 APU */
  MapperApu = Map0_Apu;

  /* Read from APU  从 APU 读取 */
  MapperReadApu = Map0_ReadApu;

  /* Callback at VSync  场同步回调 */
  MapperVSync = Map0_VSync;

  /* Callback at HSync  行同步回调 */
  MapperHSync = Map67_HSync;

  /* Callback at PPU  PPU 回调 */
  MapperPPU = Map0_PPU;

  /* Callback at Rendering Screen ( 1:BG, 0:Sprite )  渲染画面回调（1：背景，0：精灵） */
  MapperRenderScreen = Map0_RenderScreen;

  /* Set SRAM Banks  设置 SRAM 银行 */
  SRAMBANK = SRAM;

  /* Set ROM Banks  设置 ROM 银行 */
  ROMBANK0 = ROMPAGE( 0 );
  ROMBANK1 = ROMPAGE( 1 );
  ROMBANK2 = ROMLASTPAGE( 1 );
  ROMBANK3 = ROMLASTPAGE( 0 );

  /* Set PPU Banks  设置 PPU 银行 */
  PPUBANK[ 0 ] = VROMPAGE( 0 );
  PPUBANK[ 1 ] = VROMPAGE( 1 );
  PPUBANK[ 2 ] = VROMPAGE( 2 );
  PPUBANK[ 3 ] = VROMPAGE( 3 );
  PPUBANK[ 4 ] = VROMPAGE( ( NesHeader.v_rom_size << 3 ) - 4 );
  PPUBANK[ 5 ] = VROMPAGE( ( NesHeader.v_rom_size << 3 ) - 3 );
  PPUBANK[ 6 ] = VROMPAGE( ( NesHeader.v_rom_size << 3 ) - 2 );
  PPUBANK[ 7 ] = VROMPAGE( ( NesHeader.v_rom_size << 3 ) - 1 );
  InfoNES_SetupChr();

  /* Initialize IRQ Registers  初始化 IRQ 寄存器 */
  Map67_IRQ_Enable = 0;
  Map67_IRQ_Cnt = 0;
  Map67_IRQ_Latch = 0;

  /* Set up wiring of the interrupt pin  设置中断引脚接线方式 */
  K6502_Set_Int_Wiring( 1, 1 ); 
}

/*-------------------------------------------------------------------*/
/*  Mapper 67 Write Function  映射器 67：写函数                      */
/*-------------------------------------------------------------------*/
void Map67_Write( uint16_t address, uint8_t byte_data )
{
  switch ( address & 0xf800 )
  {
    /* Set PPU Banks  设置 PPU 银行 */
    case 0x8800:
      byte_data <<= 1;
      byte_data %= ( NesHeader.v_rom_size << 3 );

      PPUBANK[ 0 ] = VROMPAGE( byte_data + 0 );
      PPUBANK[ 1 ] = VROMPAGE( byte_data + 1 );
      InfoNES_SetupChr();
      break;

    case 0x9800:
      byte_data <<= 1;
      byte_data %= ( NesHeader.v_rom_size << 3 );

      PPUBANK[ 2 ] = VROMPAGE( byte_data + 0 );
      PPUBANK[ 3 ] = VROMPAGE( byte_data + 1 );
      InfoNES_SetupChr();
      break;

    case 0xa800:
      byte_data <<= 1;
      byte_data %= ( NesHeader.v_rom_size << 3 );

      PPUBANK[ 4 ] = VROMPAGE( byte_data + 0 );
      PPUBANK[ 5 ] = VROMPAGE( byte_data + 1 );
      InfoNES_SetupChr();
      break;

    case 0xb800:
      byte_data <<= 1;
      byte_data %= ( NesHeader.v_rom_size << 3 );

      PPUBANK[ 6 ] = VROMPAGE( byte_data + 0 );
      PPUBANK[ 7 ] = VROMPAGE( byte_data + 1 );
      InfoNES_SetupChr();
      break;

    case 0xc800:
      Map67_IRQ_Cnt = Map67_IRQ_Latch;
      Map67_IRQ_Latch = byte_data;
      break;

    case 0xd800:
      Map67_IRQ_Enable = byte_data & 0x10;
      break;

    case 0xe800:
      switch ( byte_data & 0x03 )
      {
        case 0:
          InfoNES_Mirroring( 1 );
          break;
        case 1:
          InfoNES_Mirroring( 0 );
          break;
        case 2:
          InfoNES_Mirroring( 3 );
          break;
        case 3:
          InfoNES_Mirroring( 2 );
          break;
      }
      break;

    /* Set ROM Banks  设置 ROM 银行 */
    case 0xf800:
      byte_data <<= 1;
      byte_data %= ( NesHeader.rom_size << 1 );

      ROMBANK0 = ROMPAGE( byte_data + 0 );
      ROMBANK1 = ROMPAGE( byte_data + 1 );
      break;
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 67 H-Sync Function  映射器 67：行同步函数                 */
/*-------------------------------------------------------------------*/
void Map67_HSync()
{
/*
 *  Callback at HSync  行同步回调
 *
 */
  if ( Map67_IRQ_Enable )
  {
    if ( 0 <= PPU_Scanline && PPU_Scanline <= 239 )
    {
      if ( PPU_R1 & R1_SHOW_SCR || PPU_R1 & R1_SHOW_SP )
      {
        if ( --Map67_IRQ_Cnt == 0xf6 )
        {
          Map67_IRQ_Cnt = Map67_IRQ_Latch;
          IRQ_REQ;
        }
      }
    }
  }
}
