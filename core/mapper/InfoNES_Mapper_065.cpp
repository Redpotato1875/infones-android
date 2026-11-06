/*===================================================================*/
/*                                                                   */
/*                      Mapper 65 (Irem H3001)  映射器 65（Irem H3001） */
/*                                                                   */
/*===================================================================*/

uint8_t  Map65_IRQ_Enable;
uint32_t Map65_IRQ_Cnt;
uint32_t Map65_IRQ_Latch;

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 65  初始化映射器 65                            */
/*-------------------------------------------------------------------*/
void Map65_Init()
{
  /* Initialize Mapper  初始化映射器 */
  MapperInit = Map65_Init;

  /* Write to Mapper  写入映射器 */
  MapperWrite = Map65_Write;

  /* Write to SRAM  写入 SRAM */
  MapperSram = Map0_Sram;

  /* Write to APU  写入 APU */
  MapperApu = Map0_Apu;

  /* Read from APU  从 APU 读取 */
  MapperReadApu = Map0_ReadApu;

  /* Callback at VSync  场同步回调 */
  MapperVSync = Map0_VSync;

  /* Callback at HSync  行同步回调 */
  MapperHSync = Map65_HSync;

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
/*  Mapper 65 Write Function  映射器 65：写函数                      */
/*-------------------------------------------------------------------*/
void Map65_Write( uint16_t address, uint8_t byte_data )
{
  switch ( address )
  {
    case 0x8000:
      ROMBANK0 = ROMPAGE( byte_data % ( NesHeader.rom_size << 1) );
      break;

    case 0x9000:
      if ( byte_data & 0x40 )
      {
        InfoNES_Mirroring( 1 );
      } else {
        InfoNES_Mirroring( 0 );
      }
      break;

    case 0x9003:
      Map65_IRQ_Enable = byte_data & 0x80;
      break;

    case 0x9004:
      Map65_IRQ_Cnt = Map65_IRQ_Latch;
      break;

    case 0x9005:
      Map65_IRQ_Latch = ( Map65_IRQ_Latch & 0x00ff ) | ((uint32_t)byte_data << 8 );
      break;

    case 0x9006:
      Map65_IRQ_Latch = ( Map65_IRQ_Latch & 0xff00 ) | (uint32_t)byte_data;
      break;

    /* Set PPU Banks  设置 PPU 银行 */
    case 0xb000:
      PPUBANK[ 0 ] = VROMPAGE( byte_data % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();
      break;

    case 0xb001:
      PPUBANK[ 1 ] = VROMPAGE( byte_data % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();
      break;

    case 0xb002:
      PPUBANK[ 2 ] = VROMPAGE( byte_data % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();
      break;

    case 0xb003:
      PPUBANK[ 3 ] = VROMPAGE( byte_data % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();
      break;

    case 0xb004:
      PPUBANK[ 4 ] = VROMPAGE( byte_data % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();
      break;

    case 0xb005:
      PPUBANK[ 5 ] = VROMPAGE( byte_data % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();
      break;

    case 0xb006:
      PPUBANK[ 6 ] = VROMPAGE( byte_data % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();
      break;

    case 0xb007:
      PPUBANK[ 7 ] = VROMPAGE( byte_data % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();
      break;

    /* Set ROM Banks  设置 ROM 银行 */
    case 0xa000:
      ROMBANK1 = ROMPAGE( byte_data % ( NesHeader.rom_size << 1) );
      break;

    case 0xc000:
      ROMBANK2 = ROMPAGE( byte_data % ( NesHeader.rom_size << 1) );
      break;
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 65 H-Sync Function  映射器 65：行同步函数                 */
/*-------------------------------------------------------------------*/
void Map65_HSync()
{
/*
 *  Callback at HSync  行同步回调
 *
 */
  if ( Map65_IRQ_Enable )
  {
    if ( Map65_IRQ_Cnt <= 113 )
    {
      IRQ_REQ;
      Map65_IRQ_Enable = 0;
      Map65_IRQ_Cnt = 0xffff;
    } else {
      Map65_IRQ_Cnt -= 113;
    }
  }
}
