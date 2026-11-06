/*===================================================================*/
/*                                                                   */
/*                           Mapper 57  映射器 57                    */
/*                                                                   */
/*===================================================================*/

uint8_t	Map57_Reg;

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 57  初始化映射器 57                            */
/*-------------------------------------------------------------------*/
void Map57_Init()
{
  /* Initialize Mapper  初始化映射器 */
  MapperInit = Map57_Init;

  /* Write to Mapper  写入映射器 */
  MapperWrite = Map57_Write;

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
  ROMBANK2 = ROMPAGE( 0 );
  ROMBANK3 = ROMPAGE( 1 );

  /* Set PPU Banks  设置 PPU 银行 */
  if ( NesHeader.v_rom_size > 0 ) {
    for ( int page_number = 0; page_number < 8; ++page_number )
      PPUBANK[ page_number ] = VROMPAGE( page_number );
    InfoNES_SetupChr();
  }

  /* Set Registers  设置寄存器 */
  Map57_Reg = 0;

  /* Set up wiring of the interrupt pin  设置中断引脚接线方式 */
  K6502_Set_Int_Wiring( 1, 1 ); 
}

/*-------------------------------------------------------------------*/
/*  Mapper 57 Write Function  映射器 57：写函数                      */
/*-------------------------------------------------------------------*/
void Map57_Write( uint16_t address, uint8_t byte_data )
{
  uint8_t chr_value;

  switch( address ) {
  case	0x8000:
  case	0x8001:
  case	0x8002:
  case	0x8003:
    if( byte_data & 0x40 ) {
      chr_value = (byte_data&0x03)+((Map57_Reg&0x10)>>1)+(Map57_Reg&0x07);

      PPUBANK[ 0 ] = VROMPAGE(((chr_value<<3)+0) % (NesHeader.v_rom_size<<3));
      PPUBANK[ 1 ] = VROMPAGE(((chr_value<<3)+1) % (NesHeader.v_rom_size<<3));
      PPUBANK[ 2 ] = VROMPAGE(((chr_value<<3)+2) % (NesHeader.v_rom_size<<3));
      PPUBANK[ 3 ] = VROMPAGE(((chr_value<<3)+3) % (NesHeader.v_rom_size<<3));
      PPUBANK[ 4 ] = VROMPAGE(((chr_value<<3)+4) % (NesHeader.v_rom_size<<3));
      PPUBANK[ 5 ] = VROMPAGE(((chr_value<<3)+5) % (NesHeader.v_rom_size<<3));
      PPUBANK[ 6 ] = VROMPAGE(((chr_value<<3)+6) % (NesHeader.v_rom_size<<3));
      PPUBANK[ 7 ] = VROMPAGE(((chr_value<<3)+7) % (NesHeader.v_rom_size<<3));
      InfoNES_SetupChr();
    }
    break;
  case	0x8800:
    Map57_Reg = byte_data;
    
    if( byte_data & 0x80 ) {
      ROMBANK0 = ROMPAGE((((byte_data & 0x40)>>6)*4+8+0) % (NesHeader.rom_size<<1));
      ROMBANK1 = ROMPAGE((((byte_data & 0x40)>>6)*4+8+1) % (NesHeader.rom_size<<1));
      ROMBANK2 = ROMPAGE((((byte_data & 0x40)>>6)*4+8+2) % (NesHeader.rom_size<<1));
      ROMBANK3 = ROMPAGE((((byte_data & 0x40)>>6)*4+8+3) % (NesHeader.rom_size<<1));
    } else {
      ROMBANK0 = ROMPAGE((((byte_data & 0x60)>>5)*2+0) % (NesHeader.rom_size<<1));
      ROMBANK1 = ROMPAGE((((byte_data & 0x60)>>5)*2+1) % (NesHeader.rom_size<<1));
      ROMBANK2 = ROMPAGE((((byte_data & 0x60)>>5)*2+0) % (NesHeader.rom_size<<1));
      ROMBANK3 = ROMPAGE((((byte_data & 0x60)>>5)*2+1) % (NesHeader.rom_size<<1));
    }
    
    chr_value = (byte_data&0x07)+((byte_data&0x10)>>1);
    
    PPUBANK[ 0 ] = VROMPAGE(((chr_value<<3)+0) % (NesHeader.v_rom_size<<3));
    PPUBANK[ 1 ] = VROMPAGE(((chr_value<<3)+1) % (NesHeader.v_rom_size<<3));
    PPUBANK[ 2 ] = VROMPAGE(((chr_value<<3)+2) % (NesHeader.v_rom_size<<3));
    PPUBANK[ 3 ] = VROMPAGE(((chr_value<<3)+3) % (NesHeader.v_rom_size<<3));
    PPUBANK[ 4 ] = VROMPAGE(((chr_value<<3)+4) % (NesHeader.v_rom_size<<3));
    PPUBANK[ 5 ] = VROMPAGE(((chr_value<<3)+5) % (NesHeader.v_rom_size<<3));
    PPUBANK[ 6 ] = VROMPAGE(((chr_value<<3)+6) % (NesHeader.v_rom_size<<3));
    PPUBANK[ 7 ] = VROMPAGE(((chr_value<<3)+7) % (NesHeader.v_rom_size<<3));
    InfoNES_SetupChr();

    if( byte_data & 0x08 ) InfoNES_Mirroring( 0 );
    else		InfoNES_Mirroring( 1 );
    
    break;
  }
}

