/*===================================================================*/
/*                                                                   */
/*                          Mapper 140  映射器 140                   */
/*                                                                   */
/*===================================================================*/

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 140  初始化映射器 140                          */
/*-------------------------------------------------------------------*/
void Map140_Init()
{
  /* Initialize Mapper  初始化映射器 */
  MapperInit = Map0_Init;

  /* Write to Mapper  写入映射器 */
  MapperWrite = Map0_Write;

  /* Write to SRAM  写入 SRAM */
  MapperSram = Map140_Sram;

  /* Write to APU  写入 APU */
  MapperApu = Map140_Apu;

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
/*  Mapper 140 Write to SRAM Function  映射器 140：写 SRAM 函数      */
/*-------------------------------------------------------------------*/
void Map140_Sram( uint16_t address, uint8_t byte_data )
{
  Map140_Apu( address, byte_data );
}

/*-------------------------------------------------------------------*/
/*  Mapper 140 Write to APU Function  映射器 140：写 APU 函数        */
/*-------------------------------------------------------------------*/
void Map140_Apu( uint16_t address, uint8_t byte_data )
{
  /* Set ROM Banks  设置 ROM 银行 */
  ROMBANK0 = ROMPAGE( (((byte_data&0xF0)>>2) + 0 ) % (NesHeader.rom_size << 1) );
  ROMBANK1 = ROMPAGE( (((byte_data%0xF0)>>2) + 1 ) % (NesHeader.rom_size << 1) );
  ROMBANK2 = ROMPAGE( (((byte_data%0xF0)>>2) + 2 ) % (NesHeader.rom_size << 1) );
  ROMBANK3 = ROMPAGE( (((byte_data%0xF0)>>2) + 3 ) % (NesHeader.rom_size << 1) );

  /* Set PPU Banks  设置 PPU 银行 */
  PPUBANK[ 0 ] = VROMPAGE( (((byte_data&0x0F)<<3) + 0) % (NesHeader.v_rom_size << 3) );
  PPUBANK[ 1 ] = VROMPAGE( (((byte_data&0x0F)<<3) + 1) % (NesHeader.v_rom_size << 3) );
  PPUBANK[ 2 ] = VROMPAGE( (((byte_data&0x0F)<<3) + 2) % (NesHeader.v_rom_size << 3) );
  PPUBANK[ 3 ] = VROMPAGE( (((byte_data&0x0F)<<3) + 3) % (NesHeader.v_rom_size << 3) );
  PPUBANK[ 4 ] = VROMPAGE( (((byte_data&0x0F)<<3) + 4) % (NesHeader.v_rom_size << 3) );
  PPUBANK[ 5 ] = VROMPAGE( (((byte_data&0x0F)<<3) + 5) % (NesHeader.v_rom_size << 3) );
  PPUBANK[ 6 ] = VROMPAGE( (((byte_data&0x0F)<<3) + 6) % (NesHeader.v_rom_size << 3) );
  PPUBANK[ 7 ] = VROMPAGE( (((byte_data&0x0F)<<3) + 7) % (NesHeader.v_rom_size << 3) );
  InfoNES_SetupChr();
}
