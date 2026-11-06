/*===================================================================*/
/*                                                                   */
/*                  Mapper 22 (Konami VRC2 type A)  映射器 22（Konami VRC2 type A） */
/*                                                                   */
/*===================================================================*/

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 22  初始化映射器 22                            */
/*-------------------------------------------------------------------*/
void Map22_Init()
{
  /* Initialize Mapper  初始化映射器 */
  MapperInit = Map22_Init;

  /* Write to Mapper  写入映射器 */
  MapperWrite = Map22_Write;

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

  /* Set up wiring of the interrupt pin  设置中断引脚接线方式 */
  K6502_Set_Int_Wiring( 1, 1 ); 
}

/*-------------------------------------------------------------------*/
/*  Mapper 22 Write Function  映射器 22：写函数                      */
/*-------------------------------------------------------------------*/
void Map22_Write( uint16_t address, uint8_t byte_data )
{
  switch ( address )
  {
    case 0x8000:
      /* Set ROM Banks  设置 ROM 银行 */
      byte_data %= ( NesHeader.rom_size << 1 );
      ROMBANK0 = ROMPAGE( byte_data );
      break;

    case 0x9000:
      /* Name Table Mirroring  命名表镜像 */
      switch ( byte_data & 0x03 )
      {
        case 0:
          InfoNES_Mirroring( 1 );   /* Vertical  垂直 */
          break;
        case 1:
          InfoNES_Mirroring( 0 );   /* Horizontal  水平 */
          break;
        case 2:
          InfoNES_Mirroring( 2 );   /* One Screen 0x2000  单屏 0x2000 */
          break;
        case 3:
          InfoNES_Mirroring( 3 );   /* One Screen 0x2400  单屏 0x2400 */
          break;
      }
      break;

    case 0xa000:
      /* Set ROM Banks  设置 ROM 银行 */
      byte_data %= ( NesHeader.rom_size << 1 );
      ROMBANK1 = ROMPAGE( byte_data );
      break;
    
    case 0xb000:
      /* Set PPU Banks  设置 PPU 银行 */
      byte_data >>= 1;
      byte_data %= ( NesHeader.v_rom_size << 3 );
      PPUBANK[ 0 ] = VROMPAGE( byte_data );
      InfoNES_SetupChr();
      break;

    case 0xb001:
      /* Set PPU Banks  设置 PPU 银行 */
      byte_data >>= 1;
      byte_data %= ( NesHeader.v_rom_size << 3 );
      PPUBANK[ 1 ] = VROMPAGE( byte_data );
      InfoNES_SetupChr();
      break;
    
    case 0xc000:
      /* Set PPU Banks  设置 PPU 银行 */
      byte_data >>= 1;
      byte_data %= ( NesHeader.v_rom_size << 3 );
      PPUBANK[ 2 ] = VROMPAGE( byte_data );
      InfoNES_SetupChr();
      break;

    case 0xc001:
      /* Set PPU Banks  设置 PPU 银行 */
      byte_data >>= 1;
      byte_data %= ( NesHeader.v_rom_size << 3 );
      PPUBANK[ 3 ] = VROMPAGE( byte_data );
      InfoNES_SetupChr();
      break;
          
    case 0xd000:
      /* Set PPU Banks  设置 PPU 银行 */
      byte_data >>= 1;
      byte_data %= ( NesHeader.v_rom_size << 3 );
      PPUBANK[ 4 ] = VROMPAGE( byte_data );
      InfoNES_SetupChr();
      break;

    case 0xd001:
      /* Set PPU Banks  设置 PPU 银行 */
      byte_data >>= 1;
      byte_data %= ( NesHeader.v_rom_size << 3 );
      PPUBANK[ 5 ] = VROMPAGE( byte_data );
      InfoNES_SetupChr();
      break;
          
    case 0xe000:
      /* Set PPU Banks  设置 PPU 银行 */
      byte_data >>= 1;
      byte_data %= ( NesHeader.v_rom_size << 3 );
      PPUBANK[ 6 ] = VROMPAGE( byte_data );
      InfoNES_SetupChr();
      break;

    case 0xe001:
      /* Set PPU Banks  设置 PPU 银行 */
      byte_data >>= 1;
      byte_data %= ( NesHeader.v_rom_size << 3 );
      PPUBANK[ 7 ] = VROMPAGE( byte_data );
      InfoNES_SetupChr();
      break; 
  }
}
