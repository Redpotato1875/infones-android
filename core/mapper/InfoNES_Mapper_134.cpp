/*===================================================================*/
/*                                                                   */
/*                          Mapper 134  映射器 134                   */
/*                                                                   */
/*===================================================================*/

uint8_t    Map134_Cmd, Map134_Prg, Map134_Chr;

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 134  初始化映射器 134                          */
/*-------------------------------------------------------------------*/
void Map134_Init()
{
  /* Initialize Mapper  初始化映射器 */
  MapperInit = Map134_Init;

  /* Write to Mapper  写入映射器 */
  MapperWrite = Map0_Write;

  /* Write to SRAM  写入 SRAM */
  MapperSram = Map0_Sram;

  /* Write to APU  写入 APU */
  MapperApu = Map134_Apu;

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
/*  Mapper 134 Write to APU Function  映射器 134：写 APU 函数        */
/*-------------------------------------------------------------------*/
void Map134_Apu( uint16_t address, uint8_t byte_data )
{
  switch( address & 0x4101 ) {
  case	0x4100:
    Map134_Cmd = byte_data & 0x07;
    break;
  case	0x4101:
    switch( Map134_Cmd ) {
    case 0:	
      Map134_Prg = 0;
      Map134_Chr = 3;
      break;
    case 4:
      Map134_Chr &= 0x3;
      Map134_Chr |= (byte_data & 0x07) << 2;
      break;
    case 5:
      Map134_Prg = byte_data & 0x07;
      break;
    case 6:
      Map134_Chr &= 0x1C;
      Map134_Chr |= byte_data & 0x3;
      break;
    case 7:
      if( byte_data & 0x01 ) InfoNES_Mirroring( 0 );
      else		  InfoNES_Mirroring( 1 );
      break;
    }
    break;
  }

  /* Set ROM Banks  设置 ROM 银行 */
  ROMBANK0 = ROMPAGE( ((Map134_Prg<<2) + 0 ) % (NesHeader.rom_size << 1) );
  ROMBANK1 = ROMPAGE( ((Map134_Prg<<2) + 1 ) % (NesHeader.rom_size << 1) );
  ROMBANK2 = ROMPAGE( ((Map134_Prg<<2) + 2 ) % (NesHeader.rom_size << 1) );
  ROMBANK3 = ROMPAGE( ((Map134_Prg<<2) + 3 ) % (NesHeader.rom_size << 1) );
  
  /* Set PPU Banks  设置 PPU 银行 */
  PPUBANK[ 0 ] = VROMPAGE( ((Map134_Chr<<3) + 0) % (NesHeader.v_rom_size << 3) );
  PPUBANK[ 1 ] = VROMPAGE( ((Map134_Chr<<3) + 1) % (NesHeader.v_rom_size << 3) );
  PPUBANK[ 2 ] = VROMPAGE( ((Map134_Chr<<3) + 2) % (NesHeader.v_rom_size << 3) );
  PPUBANK[ 3 ] = VROMPAGE( ((Map134_Chr<<3) + 3) % (NesHeader.v_rom_size << 3) );
  PPUBANK[ 4 ] = VROMPAGE( ((Map134_Chr<<3) + 4) % (NesHeader.v_rom_size << 3) );
  PPUBANK[ 5 ] = VROMPAGE( ((Map134_Chr<<3) + 5) % (NesHeader.v_rom_size << 3) );
  PPUBANK[ 6 ] = VROMPAGE( ((Map134_Chr<<3) + 6) % (NesHeader.v_rom_size << 3) );
  PPUBANK[ 7 ] = VROMPAGE( ((Map134_Chr<<3) + 7) % (NesHeader.v_rom_size << 3) );
  InfoNES_SetupChr();

  //Map134_Wram[ address & 0x1fff ] = byte_data;
}
