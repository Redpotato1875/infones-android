/*===================================================================*/
/*                                                                   */
/*                Mapper 68 (Sunsoft Mapper #4)  映射器 68（Sunsoft Mapper #4） */
/*                                                                   */
/*===================================================================*/

uint8_t Map68_Regs[4];

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 68  初始化映射器 68                             */
/*-------------------------------------------------------------------*/
void Map68_Init()
{
  /* Initialize Mapper  初始化映射器 */
  MapperInit = Map68_Init;

  /* Write to Mapper  写入映射器 */
  MapperWrite = Map68_Write;

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

  /* Initialize state flag  初始化状态标志 */
  for ( int i = 0; i < 4; i++ )
    Map68_Regs[ i ] = 0x00;

  /* Set up wiring of the interrupt pin  设置中断引脚接线方式 */
  K6502_Set_Int_Wiring( 1, 1 ); 
}

/*-------------------------------------------------------------------*/
/*  Mapper 68 Write Function  映射器 68：写函数                      */
/*-------------------------------------------------------------------*/
void Map68_Write( uint16_t address, uint8_t byte_data )
{
  switch( address )
  {
    case 0x8000:  
      /* Set PPU Banks  设置 PPU 银行 */
      byte_data %= ( NesHeader.v_rom_size << 2 );
      byte_data <<= 1;
      PPUBANK[ 0 ] = VROMPAGE( byte_data );
      PPUBANK[ 1 ] = VROMPAGE( byte_data + 1);
      InfoNES_SetupChr();
      break;

    case 0x9000:  
      /* Set PPU Banks  设置 PPU 银行 */
      byte_data %= ( NesHeader.v_rom_size << 2 );
      byte_data <<= 1;
      PPUBANK[ 2 ] = VROMPAGE( byte_data );
      PPUBANK[ 3 ] = VROMPAGE( byte_data + 1);
      InfoNES_SetupChr();
      break;

    case 0xA000:  
      /* Set PPU Banks  设置 PPU 银行 */
      byte_data %= ( NesHeader.v_rom_size << 2 );
      byte_data <<= 1;
      PPUBANK[ 4 ] = VROMPAGE( byte_data );
      PPUBANK[ 5 ] = VROMPAGE( byte_data + 1);
      InfoNES_SetupChr();
      break;

    case 0xB000:  
      /* Set PPU Banks  设置 PPU 银行 */
      byte_data %= ( NesHeader.v_rom_size << 2 );
      byte_data <<= 1;
      PPUBANK[ 6 ] = VROMPAGE( byte_data );
      PPUBANK[ 7 ] = VROMPAGE( byte_data + 1);
      InfoNES_SetupChr();
      break;

    case 0xC000:  
      Map68_Regs[ 2 ] = byte_data;
      Map68_SyncMirror();
      break;

    case 0xD000:  
      Map68_Regs[ 3 ] = byte_data;
      Map68_SyncMirror();
      break;

    case 0xE000:  
      Map68_Regs[ 0 ] = ( byte_data & 0x10 ) >> 4;
      Map68_Regs[ 1 ] = byte_data & 0x03;
      Map68_SyncMirror();
      break;

    case 0xF000:  
      /* Set ROM Banks  设置 ROM 银行 */
      byte_data %= NesHeader.rom_size;
      byte_data <<= 1;
      ROMBANK0 = ROMPAGE( byte_data );
      ROMBANK1 = ROMPAGE( byte_data + 1 );
      break;
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 68 Sub Function  映射器 68：子函数                        */
/*-------------------------------------------------------------------*/
void Map68_SyncMirror( void )
{
  if ( Map68_Regs[ 0 ] )
  {
    switch( Map68_Regs[ 1 ] )
    {
      case 0x00:
        PPUBANK[  8 ] = VROMPAGE( Map68_Regs[ 2 ] + 0x80 );
        PPUBANK[  9 ] = VROMPAGE( Map68_Regs[ 3 ] + 0x80 );
        PPUBANK[ 10 ] = VROMPAGE( Map68_Regs[ 2 ] + 0x80 );
        PPUBANK[ 11 ] = VROMPAGE( Map68_Regs[ 3 ] + 0x80 );
        break;

      case 0x01:
        PPUBANK[  8 ] = VROMPAGE( Map68_Regs[ 2 ] + 0x80 );
        PPUBANK[  9 ] = VROMPAGE( Map68_Regs[ 2 ] + 0x80 );
        PPUBANK[ 10 ] = VROMPAGE( Map68_Regs[ 3 ] + 0x80 );
        PPUBANK[ 11 ] = VROMPAGE( Map68_Regs[ 3 ] + 0x80 );
        break;

      case 0x02:
        PPUBANK[  8 ] = VROMPAGE( Map68_Regs[ 2 ] + 0x80 );
        PPUBANK[  9 ] = VROMPAGE( Map68_Regs[ 2 ] + 0x80 );
        PPUBANK[ 10 ] = VROMPAGE( Map68_Regs[ 2 ] + 0x80 );
        PPUBANK[ 11 ] = VROMPAGE( Map68_Regs[ 2 ] + 0x80 );
        break;

      case 0x03:
        PPUBANK[  8 ] = VROMPAGE( Map68_Regs[ 3 ] + 0x80 );
        PPUBANK[  9 ] = VROMPAGE( Map68_Regs[ 3 ] + 0x80 );
        PPUBANK[ 10 ] = VROMPAGE( Map68_Regs[ 3 ] + 0x80 );
        PPUBANK[ 11 ] = VROMPAGE( Map68_Regs[ 3 ] + 0x80 );
        break;
    }
    InfoNES_SetupChr();
  } 
  else 
  {
    InfoNES_Mirroring( Map68_Regs[ 1 ] );
  }
}
