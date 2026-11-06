/*===================================================================*/
/*                                                                   */
/*                    Mapper 117 (PC-Future)  映射器 117（PC-Future） */
/*                                                                   */
/*===================================================================*/

uint8_t Map117_IRQ_Line;
uint8_t Map117_IRQ_Enable1;
uint8_t Map117_IRQ_Enable2;

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 117  初始化映射器 117                          */
/*-------------------------------------------------------------------*/
void Map117_Init()
{
  /* Initialize Mapper  初始化映射器 */
  MapperInit = Map117_Init;

  /* Write to Mapper  写入映射器 */
  MapperWrite = Map117_Write;

  /* Write to SRAM  写入 SRAM */
  MapperSram = Map0_Sram;

  /* Write to APU  写入 APU */
  MapperApu = Map0_Apu;

  /* Read from APU  从 APU 读取 */
  MapperReadApu = Map0_ReadApu;

  /* Callback at VSync  场同步回调 */
  MapperVSync = Map0_VSync;

  /* Callback at HSync  行同步回调 */
  MapperHSync = Map117_HSync;

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

  /* Initialize IRQ Registers  初始化 IRQ 寄存器 */
  Map117_IRQ_Line = 0;
  Map117_IRQ_Enable1 = 0;
  Map117_IRQ_Enable2 = 1;

  /* Set up wiring of the interrupt pin  设置中断引脚接线方式 */
  K6502_Set_Int_Wiring( 1, 1 ); 
}

/*-------------------------------------------------------------------*/
/*  Mapper 117 Write Function  映射器 117：写函数                    */
/*-------------------------------------------------------------------*/
void Map117_Write( uint16_t address, uint8_t byte_data )
{
  switch ( address )
  {
  
    /* Set ROM Banks  设置 ROM 银行 */
    case 0x8000:
      byte_data %= ( NesHeader.rom_size << 1 );
      ROMBANK0 = ROMPAGE( byte_data );
      break;

    case 0x8001:
      byte_data %= ( NesHeader.rom_size << 1 );
      ROMBANK1 = ROMPAGE( byte_data );
      break;

    case 0x8002:
      byte_data %= ( NesHeader.rom_size << 1 );
      ROMBANK2 = ROMPAGE( byte_data );
      break;

    /* Set PPU Banks  设置 PPU 银行 */
    case 0xA000:
      byte_data %= ( NesHeader.v_rom_size << 3 );
      PPUBANK[ 0 ] = VROMPAGE( byte_data );
      InfoNES_SetupChr();
      break;

    case 0xA001:
      byte_data %= ( NesHeader.v_rom_size << 3 );
      PPUBANK[ 1 ] = VROMPAGE( byte_data );
      InfoNES_SetupChr();
      break;

    case 0xA002:
      byte_data %= ( NesHeader.v_rom_size << 3 );
      PPUBANK[ 2 ] = VROMPAGE( byte_data );
      InfoNES_SetupChr();
      break;

    case 0xA003:
      byte_data %= ( NesHeader.v_rom_size << 3 );
      PPUBANK[ 3 ] = VROMPAGE( byte_data );
      InfoNES_SetupChr();
      break;

    case 0xA004:
      byte_data %= ( NesHeader.v_rom_size << 3 );
      PPUBANK[ 4 ] = VROMPAGE( byte_data );
      InfoNES_SetupChr();
      break;

    case 0xA005:
      byte_data %= ( NesHeader.v_rom_size << 3 );
      PPUBANK[ 5 ] = VROMPAGE( byte_data );
      InfoNES_SetupChr();
      break;

    case 0xA006:
      byte_data %= ( NesHeader.v_rom_size << 3 );
      PPUBANK[ 6 ] = VROMPAGE( byte_data );
      InfoNES_SetupChr();
      break;

    case 0xA007:
      byte_data %= ( NesHeader.v_rom_size << 3 );
      PPUBANK[ 7 ] = VROMPAGE( byte_data );
      InfoNES_SetupChr();
      break;

    case 0xc001:
    case 0xc002:
    case 0xc003:
      Map117_IRQ_Enable1 = Map117_IRQ_Line = byte_data;
      break;

    case 0xe000:
      Map117_IRQ_Enable2 = byte_data & 0x01;
      break;
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 117 H-Sync Function  映射器 117：行同步函数               */
/*-------------------------------------------------------------------*/
void Map117_HSync()
{
  if ( Map117_IRQ_Enable1 && Map117_IRQ_Enable2 )
  {
    if ( Map117_IRQ_Line == PPU_Scanline )
    {
      Map117_IRQ_Enable1 = 0x00;
      IRQ_REQ;
    }
  }
}
