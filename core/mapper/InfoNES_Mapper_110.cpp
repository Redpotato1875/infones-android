/*===================================================================*/
/*                                                                   */
/*                          Mapper 110  映射器 110                   */
/*                                                                   */
/*===================================================================*/

uint8_t	Map110_Reg0, Map110_Reg1;

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 110  初始化映射器 110                          */
/*-------------------------------------------------------------------*/
void Map110_Init()
{
  /* Initialize Mapper  初始化映射器 */
  MapperInit = Map110_Init;

  /* Write to Mapper  写入映射器 */
  MapperWrite = Map0_Write;

  /* Write to SRAM  写入 SRAM */
  MapperSram = Map0_Sram;

  /* Write to APU  写入 APU */
  MapperApu = Map110_Apu;

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

  /* Initialize Registers  初始化寄存器 */
  Map110_Reg0 = 0;
  Map110_Reg1 = 0;

  /* Set up wiring of the interrupt pin  设置中断引脚接线方式 */
  K6502_Set_Int_Wiring( 1, 1 ); 
}

/*-------------------------------------------------------------------*/
/*  Mapper 110 Write to APU Function  映射器 110：写 APU 函数        */
/*-------------------------------------------------------------------*/
void Map110_Apu( uint16_t address, uint8_t byte_data )
{
  switch( address ) {
  case	0x4100:
    Map110_Reg1 = byte_data & 0x07;
    break;
  case	0x4101:
    switch( Map110_Reg1 ) {
    case	5:
      ROMBANK0 = ROMPAGE( ((byte_data << 2) + 0) % ( NesHeader.rom_size << 1 ) );
      ROMBANK1 = ROMPAGE( ((byte_data << 2) + 1) % ( NesHeader.rom_size << 1 ) );
      ROMBANK2 = ROMPAGE( ((byte_data << 2) + 2) % ( NesHeader.rom_size << 1 ) );
      ROMBANK3 = ROMPAGE( ((byte_data << 2) + 3) % ( NesHeader.rom_size << 1 ) );
      break;

    case	0:
      Map110_Reg0 = byte_data & 0x01;
      PPUBANK[ 0 ] = VROMPAGE(((Map110_Reg0 << 3) + 0) % (NesHeader.v_rom_size<<3));
      PPUBANK[ 1 ] = VROMPAGE(((Map110_Reg0 << 3) + 1) % (NesHeader.v_rom_size<<3));
      PPUBANK[ 2 ] = VROMPAGE(((Map110_Reg0 << 3) + 2) % (NesHeader.v_rom_size<<3));
      PPUBANK[ 3 ] = VROMPAGE(((Map110_Reg0 << 3) + 3) % (NesHeader.v_rom_size<<3));
      PPUBANK[ 4 ] = VROMPAGE(((Map110_Reg0 << 3) + 4) % (NesHeader.v_rom_size<<3));
      PPUBANK[ 5 ] = VROMPAGE(((Map110_Reg0 << 3) + 5) % (NesHeader.v_rom_size<<3));
      PPUBANK[ 6 ] = VROMPAGE(((Map110_Reg0 << 3) + 6) % (NesHeader.v_rom_size<<3));
      PPUBANK[ 7 ] = VROMPAGE(((Map110_Reg0 << 3) + 7) % (NesHeader.v_rom_size<<3));
      InfoNES_SetupChr();
      break;

    case	2:
      Map110_Reg0 = byte_data;
      PPUBANK[ 0 ] = VROMPAGE(((Map110_Reg0 << 3) + 0) % (NesHeader.v_rom_size<<3));
      PPUBANK[ 1 ] = VROMPAGE(((Map110_Reg0 << 3) + 1) % (NesHeader.v_rom_size<<3));
      PPUBANK[ 2 ] = VROMPAGE(((Map110_Reg0 << 3) + 2) % (NesHeader.v_rom_size<<3));
      PPUBANK[ 3 ] = VROMPAGE(((Map110_Reg0 << 3) + 3) % (NesHeader.v_rom_size<<3));
      PPUBANK[ 4 ] = VROMPAGE(((Map110_Reg0 << 3) + 4) % (NesHeader.v_rom_size<<3));
      PPUBANK[ 5 ] = VROMPAGE(((Map110_Reg0 << 3) + 5) % (NesHeader.v_rom_size<<3));
      PPUBANK[ 6 ] = VROMPAGE(((Map110_Reg0 << 3) + 6) % (NesHeader.v_rom_size<<3));
      PPUBANK[ 7 ] = VROMPAGE(((Map110_Reg0 << 3) + 7) % (NesHeader.v_rom_size<<3));
      InfoNES_SetupChr();
      break;

    case	4:
      Map110_Reg0 = Map110_Reg0 | (byte_data<<1);
      PPUBANK[ 0 ] = VROMPAGE(((Map110_Reg0 << 3) + 0) % (NesHeader.v_rom_size<<3));
      PPUBANK[ 1 ] = VROMPAGE(((Map110_Reg0 << 3) + 1) % (NesHeader.v_rom_size<<3));
      PPUBANK[ 2 ] = VROMPAGE(((Map110_Reg0 << 3) + 2) % (NesHeader.v_rom_size<<3));
      PPUBANK[ 3 ] = VROMPAGE(((Map110_Reg0 << 3) + 3) % (NesHeader.v_rom_size<<3));
      PPUBANK[ 4 ] = VROMPAGE(((Map110_Reg0 << 3) + 4) % (NesHeader.v_rom_size<<3));
      PPUBANK[ 5 ] = VROMPAGE(((Map110_Reg0 << 3) + 5) % (NesHeader.v_rom_size<<3));
      PPUBANK[ 6 ] = VROMPAGE(((Map110_Reg0 << 3) + 6) % (NesHeader.v_rom_size<<3));
      PPUBANK[ 7 ] = VROMPAGE(((Map110_Reg0 << 3) + 7) % (NesHeader.v_rom_size<<3));
      InfoNES_SetupChr();
      break;

    case	6:
      Map110_Reg0 = Map110_Reg0 | (byte_data<<2);
      PPUBANK[ 0 ] = VROMPAGE(((Map110_Reg0 << 3) + 0) % (NesHeader.v_rom_size<<3));
      PPUBANK[ 1 ] = VROMPAGE(((Map110_Reg0 << 3) + 1) % (NesHeader.v_rom_size<<3));
      PPUBANK[ 2 ] = VROMPAGE(((Map110_Reg0 << 3) + 2) % (NesHeader.v_rom_size<<3));
      PPUBANK[ 3 ] = VROMPAGE(((Map110_Reg0 << 3) + 3) % (NesHeader.v_rom_size<<3));
      PPUBANK[ 4 ] = VROMPAGE(((Map110_Reg0 << 3) + 4) % (NesHeader.v_rom_size<<3));
      PPUBANK[ 5 ] = VROMPAGE(((Map110_Reg0 << 3) + 5) % (NesHeader.v_rom_size<<3));
      PPUBANK[ 6 ] = VROMPAGE(((Map110_Reg0 << 3) + 6) % (NesHeader.v_rom_size<<3));
      PPUBANK[ 7 ] = VROMPAGE(((Map110_Reg0 << 3) + 7) % (NesHeader.v_rom_size<<3));
      InfoNES_SetupChr();
      break;

    default:
      break;
    }
    break;
  default:
    break;
  }
}
