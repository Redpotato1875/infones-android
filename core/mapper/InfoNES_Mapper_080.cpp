/*===================================================================*/
/*                                                                   */
/*                        Mapper 80 (X1-005)  映射器 80（X1-005）    */
/*                                                                   */
/*===================================================================*/

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 80  初始化映射器 80                            */
/*-------------------------------------------------------------------*/
void Map80_Init()
{
  /* Initialize Mapper  初始化映射器 */
  MapperInit = Map80_Init;

  /* Write to Mapper  写入映射器 */
  MapperWrite = Map0_Write;

  /* Write to SRAM  写入 SRAM */
  MapperSram = Map80_Sram;

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
/*  Mapper 80 Write to SRAM Function  映射器 80：写 SRAM 函数        */
/*-------------------------------------------------------------------*/
void Map80_Sram( uint16_t address, uint8_t byte_data )
{
  switch ( address )
  {
    /* Set PPU Banks  设置 PPU 银行 */
    case 0x7ef0:
      byte_data &= 0x7f;
      byte_data %= ( NesHeader.v_rom_size << 3 );
      
      PPUBANK[ 0 ] = VROMPAGE( byte_data );
      PPUBANK[ 1 ] = VROMPAGE( byte_data + 1 );
      InfoNES_SetupChr();
      break;

    case 0x7ef1:
      byte_data &= 0x7f;
      byte_data %= ( NesHeader.v_rom_size << 3 );
      
      PPUBANK[ 2 ] = VROMPAGE( byte_data );
      PPUBANK[ 3 ] = VROMPAGE( byte_data + 1 );
      InfoNES_SetupChr();
      break;
  
    case 0x7ef2:
      byte_data %= ( NesHeader.v_rom_size << 3 );
      PPUBANK[ 4 ] = VROMPAGE( byte_data );
      InfoNES_SetupChr();
      break;      

    case 0x7ef3:
      byte_data %= ( NesHeader.v_rom_size << 3 );
      PPUBANK[ 5 ] = VROMPAGE( byte_data );
      InfoNES_SetupChr();
      break;  

    case 0x7ef4:
      byte_data %= ( NesHeader.v_rom_size << 3 );
      PPUBANK[ 6 ] = VROMPAGE( byte_data );
      InfoNES_SetupChr();
      break; 

    case 0x7ef5:
      byte_data %= ( NesHeader.v_rom_size << 3 );
      PPUBANK[ 7 ] = VROMPAGE( byte_data );
      InfoNES_SetupChr();
      break; 

    /* Name Table Mirroring  命名表镜像 */
    case 0x7ef6:
      if ( byte_data & 0x01 )
      {
        InfoNES_Mirroring( 1 );
      } else {
        InfoNES_Mirroring( 0 );
      }

    /* Set ROM Banks  设置 ROM 银行 */
    case 0x7efa:
    case 0x7efb:
      byte_data %= ( NesHeader.rom_size << 1 );
      ROMBANK0 = ROMPAGE( byte_data );
      break;

    case 0x7efc:
    case 0x7efd:
      byte_data %= ( NesHeader.rom_size << 1 );
      ROMBANK1 = ROMPAGE( byte_data );
      break;

    case 0x7efe:
    case 0x7eff:
      byte_data %= ( NesHeader.rom_size << 1 );
      ROMBANK2 = ROMPAGE( byte_data );
      break;
  }
}
