/*===================================================================*/
/*                                                                   */
/*                     Mapper 234 : Maxi-15  映射器 234：Maxi-15     */
/*                                                                   */
/*===================================================================*/

uint8_t	Map234_Reg[2];

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 234  初始化映射器 234                          */
/*-------------------------------------------------------------------*/
void Map234_Init()
{
  /* Initialize Mapper  初始化映射器 */
  MapperInit = Map234_Init;

  /* Write to Mapper  写入映射器 */
  MapperWrite = Map234_Write;

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

  /* Set Registers  设置寄存器 */
  Map234_Reg[0] = 0;
  Map234_Reg[1] = 0;

  /* Set up wiring of the interrupt pin  设置中断引脚接线方式 */
  K6502_Set_Int_Wiring( 1, 1 ); 
}

/*-------------------------------------------------------------------*/
/*  Mapper 234 Write Function  映射器 234：写函数                    */
/*-------------------------------------------------------------------*/
void Map234_Write( uint16_t address, uint8_t byte_data )
{
  if( address >= 0xFF80 && address <= 0xFF9F ) {
    if( !Map234_Reg[0] ) {
      Map234_Reg[0] = byte_data;
      Map234_Set_Banks();
    }
  }
  
  if( address >= 0xFFE8 && address <= 0xFFF7 ) {
    Map234_Reg[1] = byte_data;
    Map234_Set_Banks();
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 234 Set Banks Function  映射器 234：设置银行函数          */
/*-------------------------------------------------------------------*/
void Map234_Set_Banks()
{
  uint8_t prg, chr_value;

  if( Map234_Reg[0] & 0x80 ) {
    InfoNES_Mirroring( 0 );
  } else {
    InfoNES_Mirroring( 1 );
  }
  if( Map234_Reg[0] & 0x40 ) {
    prg = (Map234_Reg[0]&0x0E)|(Map234_Reg[1]&0x01);
    chr_value = ((Map234_Reg[0]&0x0E)<<2)|((Map234_Reg[1]>>4)&0x07);
  } else {
    prg = Map234_Reg[0]&0x0F;
    chr_value = ((Map234_Reg[0]&0x0F)<<2)|((Map234_Reg[1]>>4)&0x03);
  }
  
  /* Set ROM Banks  设置 ROM 银行 */
  ROMBANK0 = ROMPAGE(((prg<<2)+0) % (NesHeader.rom_size<<1));
  ROMBANK1 = ROMPAGE(((prg<<2)+1) % (NesHeader.rom_size<<1));
  ROMBANK2 = ROMPAGE(((prg<<2)+2) % (NesHeader.rom_size<<1));
  ROMBANK3 = ROMPAGE(((prg<<2)+3) % (NesHeader.rom_size<<1));

  /* Set PPU Banks  设置 PPU 银行 */
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


