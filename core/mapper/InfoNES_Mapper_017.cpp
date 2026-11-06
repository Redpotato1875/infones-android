/*===================================================================*/
/*                                                                   */
/*                 Mapper 17 (FFE F8 Series)  映射器 17（FFE F8 Series） */
/*                                                                   */
/*===================================================================*/

uint8_t  Map17_IRQ_Enable;
uint32_t Map17_IRQ_Cnt;
uint32_t Map17_IRQ_Latch;

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 17  初始化映射器 17                            */
/*-------------------------------------------------------------------*/
void Map17_Init()
{
  /* Initialize Mapper  初始化映射器 */
  MapperInit = Map17_Init;

  /* Write to Mapper  写入映射器 */
  MapperWrite = Map0_Write;

  /* Write to SRAM  写入 SRAM */
  MapperSram = Map0_Sram;

  /* Write to APU  写入 APU */
  MapperApu = Map17_Apu;

  /* Read from APU  从 APU 读取 */
  MapperReadApu = Map0_ReadApu;

  /* Callback at VSync  场同步回调 */
  MapperVSync = Map0_VSync;

  /* Callback at HSync  行同步回调 */
  MapperHSync = Map17_HSync;

  /* Callback at PPU  PPU 回调 */
  MapperPPU = Map0_PPU;

  /* Set SRAM Banks  设置 SRAM 银行 */
  SRAMBANK = SRAM;

  /* Callback at Rendering Screen ( 1:BG, 0:Sprite )  渲染画面回调（1：背景，0：精灵） */
  MapperRenderScreen = Map0_RenderScreen;

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

  /* Initialize State Registers  初始化状态寄存器 */
  Map17_IRQ_Enable = 0;
  Map17_IRQ_Cnt = 0;
  Map17_IRQ_Latch = 0;

  /* Set up wiring of the interrupt pin  设置中断引脚接线方式 */
  K6502_Set_Int_Wiring( 1, 1 ); 
}

/*-------------------------------------------------------------------*/
/*  Mapper 17 Write to APU Function  映射器 17：写 APU 函数          */
/*-------------------------------------------------------------------*/
void Map17_Apu( uint16_t address, uint8_t byte_data )
{
  switch ( address )
  {
    case 0x42fe:
      if ( ( byte_data & 0x10 ) == 0 )
      {
        InfoNES_Mirroring( 3 );
      } else {
        InfoNES_Mirroring( 2 );
      }
      break;

    case 0x42ff:
      if ( ( byte_data & 0x10 ) == 0 )
      {
        InfoNES_Mirroring( 1 );
      } else {
        InfoNES_Mirroring( 0 );
      }
      break;

    case 0x4501:
      Map17_IRQ_Enable = 0;
      break;

    case 0x4502:
      Map17_IRQ_Latch = ( Map17_IRQ_Latch & 0xff00 ) | byte_data;
      break;

    case 0x4503:
      Map17_IRQ_Latch = ( Map17_IRQ_Latch & 0x00ff ) | ( (uint32_t)byte_data << 8 );
      Map17_IRQ_Cnt = Map17_IRQ_Latch;
      Map17_IRQ_Enable = 1;
      break;

    case 0x4504:
      byte_data %= ( NesHeader.rom_size << 1 );
      ROMBANK0 = ROMPAGE( byte_data );
      break;

    case 0x4505:
      byte_data %= ( NesHeader.rom_size << 1 );
      ROMBANK1 = ROMPAGE( byte_data );
      break;

    case 0x4506:
      byte_data %= ( NesHeader.rom_size << 1 );
      ROMBANK2 = ROMPAGE( byte_data );
      break;

    case 0x4507:
      byte_data %= ( NesHeader.rom_size << 1 );
      ROMBANK3 = ROMPAGE( byte_data );
      break;

    case 0x4510:
      byte_data %= ( NesHeader.v_rom_size << 3 );
      PPUBANK[ 0 ] = VROMPAGE( byte_data );
      InfoNES_SetupChr();
      break;

    case 0x4511:
      byte_data %= ( NesHeader.v_rom_size << 3 );
      PPUBANK[ 1 ] = VROMPAGE( byte_data );
      InfoNES_SetupChr();
      break;

    case 0x4512:
      byte_data %= ( NesHeader.v_rom_size << 3 );
      PPUBANK[ 2 ] = VROMPAGE( byte_data );
      InfoNES_SetupChr();
      break;

    case 0x4513:
      byte_data %= ( NesHeader.v_rom_size << 3 );
      PPUBANK[ 3 ] = VROMPAGE( byte_data );
      InfoNES_SetupChr();
      break;

    case 0x4514:
      byte_data %= ( NesHeader.v_rom_size << 3 );
      PPUBANK[ 4 ] = VROMPAGE( byte_data );
      InfoNES_SetupChr();
      break;

    case 0x4515:
      byte_data %= ( NesHeader.v_rom_size << 3 );
      PPUBANK[ 5 ] = VROMPAGE( byte_data );
      InfoNES_SetupChr();
      break;

    case 0x4516:
      byte_data %= ( NesHeader.v_rom_size << 3 );
      PPUBANK[ 6 ] = VROMPAGE( byte_data );
      InfoNES_SetupChr();
      break;

    case 0x4517:
      byte_data %= ( NesHeader.v_rom_size << 3 );
      PPUBANK[ 7 ] = VROMPAGE( byte_data );
      InfoNES_SetupChr();
      break;
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 17 H-Sync Function  映射器 17：行同步函数                 */
/*-------------------------------------------------------------------*/
void Map17_HSync()
{
  if ( Map17_IRQ_Enable )
  {
    if ( Map17_IRQ_Cnt >= 0xffff - 113 )
    {
      IRQ_REQ;
      Map17_IRQ_Cnt = 0;
      Map17_IRQ_Enable = 0;
    } else {
      Map17_IRQ_Cnt += 113;
    }
  }
}
