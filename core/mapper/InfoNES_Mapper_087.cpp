/*===================================================================*/
/*                                                                   */
/*                     Mapper 87 (74161/32)  映射器 87（74161/32）   */
/*                                                                   */
/*===================================================================*/

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 87  初始化映射器 87                            */
/*-------------------------------------------------------------------*/
void Map87_Init()
{
  /* Initialize Mapper  初始化映射器 */
  MapperInit = Map87_Init;

  /* Write to Mapper  写入映射器 */
  MapperWrite = Map0_Write;

  /* Write to SRAM  写入 SRAM */
  MapperSram = Map87_Sram;

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
    for ( int page_number = 0; page_number < 8; ++page_number )
      PPUBANK[ page_number ] = VROMPAGE( page_number );
    InfoNES_SetupChr();
  }

  /* Set up wiring of the interrupt pin  设置中断引脚接线方式 */
  K6502_Set_Int_Wiring( 1, 1 ); 
}

/*-------------------------------------------------------------------*/
/*  Mapper 87 Write to SRAM Function  映射器 87：写 SRAM 函数        */
/*-------------------------------------------------------------------*/
void Map87_Sram( uint16_t address, uint8_t byte_data )
{
  uint8_t chr_bank;

  switch ( address )
  {
    case 0x6000:
      chr_bank = ( byte_data & 0x02 ) >> 1;
      chr_bank <<= 3;
      chr_bank %= ( NesHeader.v_rom_size << 3 );

      PPUBANK[ 0 ] = VROMPAGE( chr_bank + 0 );
      PPUBANK[ 1 ] = VROMPAGE( chr_bank + 1 );
      PPUBANK[ 2 ] = VROMPAGE( chr_bank + 2 );
      PPUBANK[ 3 ] = VROMPAGE( chr_bank + 3 );
      PPUBANK[ 4 ] = VROMPAGE( chr_bank + 4 );
      PPUBANK[ 5 ] = VROMPAGE( chr_bank + 5 );
      PPUBANK[ 6 ] = VROMPAGE( chr_bank + 6 );
      PPUBANK[ 7 ] = VROMPAGE( chr_bank + 7 );
      InfoNES_SetupChr();
      break;
  }
}
