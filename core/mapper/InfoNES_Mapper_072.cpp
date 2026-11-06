/*===================================================================*/
/*                                                                   */
/*           Mapper 72 (Jaleco Early Mapper #0)  映射器 72（Jaleco Early Mapper #0） */
/*                                                                   */
/*===================================================================*/

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 72  初始化映射器 72                            */
/*-------------------------------------------------------------------*/
void Map72_Init()
{
  /* Initialize Mapper  初始化映射器 */
  MapperInit = Map72_Init;

  /* Write to Mapper  写入映射器 */
  MapperWrite = Map72_Write;

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
/*  Mapper 72 Write Function  映射器 72：写函数                      */
/*-------------------------------------------------------------------*/
void Map72_Write( uint16_t address, uint8_t byte_data )
{
  uint8_t bank_number = byte_data & 0x0f;

  if ( byte_data & 0x80 )
  {
    /* Set ROM Banks  设置 ROM 银行 */
    bank_number <<= 1;
    bank_number %= ( NesHeader.rom_size << 1 );
    ROMBANK0 = ROMPAGE( bank_number );
    ROMBANK1 = ROMPAGE( bank_number + 1 );
  } else 
  if ( byte_data & 0x40 )
  {
    /* Set PPU Banks  设置 PPU 银行 */
    bank_number <<= 3;
    bank_number %= ( NesHeader.v_rom_size << 3 );
    PPUBANK[ 0 ] = VROMPAGE( bank_number );
    PPUBANK[ 1 ] = VROMPAGE( bank_number + 1 );
    PPUBANK[ 2 ] = VROMPAGE( bank_number + 2 );
    PPUBANK[ 3 ] = VROMPAGE( bank_number + 3 );
    PPUBANK[ 4 ] = VROMPAGE( bank_number + 4 );
    PPUBANK[ 5 ] = VROMPAGE( bank_number + 5 );
    PPUBANK[ 6 ] = VROMPAGE( bank_number + 6 );
    PPUBANK[ 7 ] = VROMPAGE( bank_number + 7 );
    InfoNES_SetupChr();
  }
}
