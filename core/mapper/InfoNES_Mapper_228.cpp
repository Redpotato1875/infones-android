/*===================================================================*/
/*                                                                   */
/*                  Mapper 228 : Action 52  映射器 228：Action 52    */
/*                                                                   */
/*===================================================================*/

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 228  初始化映射器 228                          */
/*-------------------------------------------------------------------*/
void Map228_Init()
{
  /* Initialize Mapper  初始化映射器 */
  MapperInit = Map228_Init;

  /* Write to Mapper  写入映射器 */
  MapperWrite = Map228_Write;

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
/*  Mapper 228 Write Function  映射器 228：写函数                    */
/*-------------------------------------------------------------------*/
void Map228_Write( uint16_t address, uint8_t byte_data )
{
  uint8_t	prg = (address&0x0780)>>7;

  switch( (address&0x1800)>>11 ) {
  case	1:
    prg |= 0x10;
    break;
  case	3:
    prg |= 0x20;
    break;
  }

  if( address & 0x0020 ) {
    prg <<= 1;
    if( address & 0x0040 ) {
      prg++;
    }
    ROMBANK0 = ROMPAGE(((prg<<2)+0) % (NesHeader.rom_size<<1));
    ROMBANK1 = ROMPAGE(((prg<<2)+1) % (NesHeader.rom_size<<1));
    ROMBANK2 = ROMPAGE(((prg<<2)+0) % (NesHeader.rom_size<<1));
    ROMBANK3 = ROMPAGE(((prg<<2)+1) % (NesHeader.rom_size<<1));
  } else {
    ROMBANK0 = ROMPAGE(((prg<<2)+0) % (NesHeader.rom_size<<1));
    ROMBANK1 = ROMPAGE(((prg<<2)+1) % (NesHeader.rom_size<<1));
    ROMBANK2 = ROMPAGE(((prg<<2)+2) % (NesHeader.rom_size<<1));
    ROMBANK3 = ROMPAGE(((prg<<2)+3) % (NesHeader.rom_size<<1));
  }

  uint8_t chr_value = ((address&0x000F)<<2)|(byte_data&0x03);

  PPUBANK[ 0 ] = VROMPAGE(((chr_value<<3)+0) % (NesHeader.v_rom_size<<3)); 
  PPUBANK[ 1 ] = VROMPAGE(((chr_value<<3)+1) % (NesHeader.v_rom_size<<3)); 
  PPUBANK[ 2 ] = VROMPAGE(((chr_value<<3)+2) % (NesHeader.v_rom_size<<3)); 
  PPUBANK[ 3 ] = VROMPAGE(((chr_value<<3)+3) % (NesHeader.v_rom_size<<3)); 
  PPUBANK[ 4 ] = VROMPAGE(((chr_value<<3)+4) % (NesHeader.v_rom_size<<3)); 
  PPUBANK[ 5 ] = VROMPAGE(((chr_value<<3)+5) % (NesHeader.v_rom_size<<3)); 
  PPUBANK[ 6 ] = VROMPAGE(((chr_value<<3)+6) % (NesHeader.v_rom_size<<3)); 
  PPUBANK[ 7 ] = VROMPAGE(((chr_value<<3)+7) % (NesHeader.v_rom_size<<3)); 
  InfoNES_SetupChr();
  
  if( address & 0x2000 ) {
    InfoNES_Mirroring( 0 );
  } else {
    InfoNES_Mirroring( 1 );
  }
}
