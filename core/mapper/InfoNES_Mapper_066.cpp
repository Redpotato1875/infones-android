/*===================================================================*/
/*                                                                   */
/*                        Mapper 66 (GNROM)  映射器 66（GNROM）      */
/*                                                                   */
/*===================================================================*/

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 66  初始化映射器 66                            */
/*-------------------------------------------------------------------*/
void Map66_Init()
{
  int page_number;

  /* Initialize Mapper  初始化映射器 */
  MapperInit = Map66_Init;

  /* Write to Mapper  写入映射器 */
  MapperWrite = Map66_Write;

  /* Write to SRAM  写入 SRAM */
  MapperSram = Map66_Write;

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
  ROMBANK2 = ROMPAGE( 2 );
  ROMBANK3 = ROMPAGE( 3 );

  /* Set PPU Banks  设置 PPU 银行 */
  if ( NesHeader.v_rom_size > 0 )
  {
    for ( page_number = 0; page_number < 8; ++page_number )
      PPUBANK[ page_number ] = VROMPAGE( page_number );
    InfoNES_SetupChr();
  }

  /* Set up wiring of the interrupt pin  设置中断引脚接线方式 */
  K6502_Set_Int_Wiring( 1, 0 ); 
}

/*-------------------------------------------------------------------*/
/*  Mapper 66 Write Function  映射器 66：写函数                      */
/*-------------------------------------------------------------------*/
void Map66_Write( uint16_t address, uint8_t byte_data )
{
  uint8_t rom;
  uint8_t v_rom;

  rom  = ( byte_data >> 4 ) & 0x0F;
  v_rom = byte_data & 0x0F;

  /* Set ROM Banks  设置 ROM 银行 */
  rom <<= 1;
  rom %= NesHeader.rom_size;
  rom <<= 1;

  ROMBANK0 = ROMPAGE( rom );
  ROMBANK1 = ROMPAGE( rom + 1 );
  ROMBANK2 = ROMPAGE( rom + 2 );
  ROMBANK3 = ROMPAGE( rom + 3 );

  /* Set PPU Banks  设置 PPU 银行 */
  v_rom <<= 3;
  v_rom %= ( NesHeader.v_rom_size << 3 );

  PPUBANK[ 0 ] = VROMPAGE( v_rom );
  PPUBANK[ 1 ] = VROMPAGE( v_rom + 1 );
  PPUBANK[ 2 ] = VROMPAGE( v_rom + 2 );
  PPUBANK[ 3 ] = VROMPAGE( v_rom + 3 );
  PPUBANK[ 4 ] = VROMPAGE( v_rom + 4 );
  PPUBANK[ 5 ] = VROMPAGE( v_rom + 5 );
  PPUBANK[ 6 ] = VROMPAGE( v_rom + 6 );
  PPUBANK[ 7 ] = VROMPAGE( v_rom + 7 );
  InfoNES_SetupChr();
}
