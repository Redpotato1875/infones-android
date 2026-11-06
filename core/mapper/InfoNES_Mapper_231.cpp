/*===================================================================*/
/*                                                                   */
/*                      Mapper 231 : 20-in-1  映射器 231：20 合 1    */
/*                                                                   */
/*===================================================================*/

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 231  初始化映射器 231                          */
/*-------------------------------------------------------------------*/
void Map231_Init()
{
  /* Initialize Mapper  初始化映射器 */
  MapperInit = Map231_Init;

  /* Write to Mapper  写入映射器 */
  MapperWrite = Map231_Write;

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
  ROMBANK2 = ROMPAGE( 2 );
  ROMBANK3 = ROMPAGE( 3 );

  /* Set PPU Banks  设置 PPU 银行 */
  if ( NesHeader.v_rom_size > 0 ) {
    for ( int page_number = 0; page_number < 8; ++page_number )
      PPUBANK[ page_number ] = VROMPAGE( page_number );
    InfoNES_SetupChr();
  }

  /* Set up wiring of the interrupt pin  设置中断引脚接线方式 */
  K6502_Set_Int_Wiring( 1, 1 ); 
}

/*-------------------------------------------------------------------*/
/*  Mapper 231 Write Function  映射器 231：写函数                    */
/*-------------------------------------------------------------------*/
void Map231_Write( uint16_t address, uint8_t byte_data )
{
  uint8_t	bank_number = address & 0x1E;

  if( address & 0x0020 ) {
    ROMBANK0 = ROMPAGE(((bank_number<<1)+0) % (NesHeader.rom_size<<1));
    ROMBANK1 = ROMPAGE(((bank_number<<1)+1) % (NesHeader.rom_size<<1));
    ROMBANK2 = ROMPAGE(((bank_number<<1)+2) % (NesHeader.rom_size<<1));
    ROMBANK3 = ROMPAGE(((bank_number<<1)+3) % (NesHeader.rom_size<<1));
  } else {
    ROMBANK0 = ROMPAGE(((bank_number<<1)+0) % (NesHeader.rom_size<<1));
    ROMBANK1 = ROMPAGE(((bank_number<<1)+1) % (NesHeader.rom_size<<1));
    ROMBANK2 = ROMPAGE(((bank_number<<1)+0) % (NesHeader.rom_size<<1));
    ROMBANK3 = ROMPAGE(((bank_number<<1)+1) % (NesHeader.rom_size<<1));
  }

  if( address & 0x0080 ) {
    InfoNES_Mirroring( 0 );
  } else {
    InfoNES_Mirroring( 1 );
  }
}
