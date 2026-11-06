/*===================================================================*/
/*                                                                   */
/*           Mapper 75 (Konami VRC 1 and Jaleco SS8805)  映射器 75（Konami VRC1 与 Jaleco SS8805） */
/*                                                                   */
/*===================================================================*/

uint8_t Map75_Regs[ 2 ];

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 75  初始化映射器 75                            */
/*-------------------------------------------------------------------*/
void Map75_Init()
{
  /* Initialize Mapper  初始化映射器 */
  MapperInit = Map75_Init;

  /* Write to Mapper  写入映射器 */
  MapperWrite = Map75_Write;

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

  /* Initialize State Flag  初始化状态标志 */
  Map75_Regs[ 0 ] = 0;
  Map75_Regs[ 1 ] = 1;

  /* Set up wiring of the interrupt pin  设置中断引脚接线方式 */
  K6502_Set_Int_Wiring( 1, 1 ); 
}

/*-------------------------------------------------------------------*/
/*  Mapper 75 Write Function  映射器 75：写函数                      */
/*-------------------------------------------------------------------*/
void Map75_Write( uint16_t address, uint8_t byte_data )
{
  switch ( address & 0xf000 )
  {
    /* Set ROM Banks  设置 ROM 银行 */
    case 0x8000:
      byte_data %= ( NesHeader.rom_size << 1 );
      ROMBANK0 = ROMPAGE( byte_data );
      break;

    case 0x9000:
      /* Set Mirroring  设置镜像 */
      if ( byte_data & 0x01 )
      {
        InfoNES_Mirroring( 0 );
      } else {
        InfoNES_Mirroring( 1 );
      }

      /* Set PPU Banks  设置 PPU 银行 */
      Map75_Regs[ 0 ] = ( Map75_Regs[ 0 ] & 0x0f ) | ( ( byte_data & 0x02 ) << 3 );
      PPUBANK[ 0 ] = VROMPAGE( ( Map75_Regs[ 0 ] << 2 ) + 0 );
      PPUBANK[ 1 ] = VROMPAGE( ( Map75_Regs[ 0 ] << 2 ) + 1 );
      PPUBANK[ 2 ] = VROMPAGE( ( Map75_Regs[ 0 ] << 2 ) + 2 );
      PPUBANK[ 3 ] = VROMPAGE( ( Map75_Regs[ 0 ] << 2 ) + 3 );

      Map75_Regs[ 1 ] = ( Map75_Regs[ 1 ] & 0x0f ) | ( ( byte_data & 0x04 ) << 2 );
      PPUBANK[ 4 ] = VROMPAGE( ( Map75_Regs[ 1 ] << 2 ) + 0 );
      PPUBANK[ 5 ] = VROMPAGE( ( Map75_Regs[ 1 ] << 2 ) + 1 );
      PPUBANK[ 6 ] = VROMPAGE( ( Map75_Regs[ 1 ] << 2 ) + 2 );
      PPUBANK[ 7 ] = VROMPAGE( ( Map75_Regs[ 1 ] << 2 ) + 3 );
      InfoNES_SetupChr();
      break;

    /* Set ROM Banks  设置 ROM 银行 */
    case 0xA000:
      byte_data %= ( NesHeader.rom_size << 1 );
      ROMBANK1 = ROMPAGE( byte_data );
      break;

    /* Set ROM Banks  设置 ROM 银行 */
    case 0xC000:
      byte_data %= ( NesHeader.rom_size << 1 );
      ROMBANK2 = ROMPAGE( byte_data );
      break;

    case 0xE000:
      /* Set PPU Banks  设置 PPU 银行 */
      Map75_Regs[ 0 ] = ( Map75_Regs[ 0 ] & 0x10 ) | ( byte_data & 0x0f );
      PPUBANK[ 0 ] = VROMPAGE( ( Map75_Regs[ 0 ] << 2 ) + 0 );
      PPUBANK[ 1 ] = VROMPAGE( ( Map75_Regs[ 0 ] << 2 ) + 1 );
      PPUBANK[ 2 ] = VROMPAGE( ( Map75_Regs[ 0 ] << 2 ) + 2 );
      PPUBANK[ 3 ] = VROMPAGE( ( Map75_Regs[ 0 ] << 2 ) + 3 );
      InfoNES_SetupChr();
      break;

    case 0xF000:
      /* Set PPU Banks  设置 PPU 银行 */
      Map75_Regs[ 1 ] = ( Map75_Regs[ 1 ] & 0x10 ) | ( byte_data & 0x0f );
      PPUBANK[ 4 ] = VROMPAGE( ( Map75_Regs[ 1 ] << 2 ) + 0 );
      PPUBANK[ 5 ] = VROMPAGE( ( Map75_Regs[ 1 ] << 2 ) + 1 );
      PPUBANK[ 6 ] = VROMPAGE( ( Map75_Regs[ 1 ] << 2 ) + 2 );
      PPUBANK[ 7 ] = VROMPAGE( ( Map75_Regs[ 1 ] << 2 ) + 3 );
      InfoNES_SetupChr();
      break;
  }
}
