/*===================================================================*/
/*                                                                   */
/*                   Mapper 226 : 76-in-1  映射器 226：76 合 1       */
/*                                                                   */
/*===================================================================*/

uint8_t	Map226_Reg[2];

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 226  初始化映射器 226                          */
/*-------------------------------------------------------------------*/
void Map226_Init()
{
  /* Initialize Mapper  初始化映射器 */
  MapperInit = Map226_Init;

  /* Write to Mapper  写入映射器 */
  MapperWrite = Map226_Write;

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

  /* Initialize Registers  初始化寄存器 */
  Map226_Reg[0] = 0;
  Map226_Reg[1] = 0;
  
  /* Set up wiring of the interrupt pin  设置中断引脚接线方式 */
  K6502_Set_Int_Wiring( 1, 1 ); 
}

/*-------------------------------------------------------------------*/
/*  Mapper 226 Write Function  映射器 226：写函数                    */
/*-------------------------------------------------------------------*/
void Map226_Write( uint16_t address, uint8_t byte_data )
{
  if( address & 0x0001 ) {
    Map226_Reg[1] = byte_data;
  } else {
    Map226_Reg[0] = byte_data;
  }
  
  if( Map226_Reg[0] & 0x40 ) {
    InfoNES_Mirroring( 1 );
  } else {
    InfoNES_Mirroring( 0 );
  }
  
  uint8_t	bank_number = ((Map226_Reg[0]&0x1E)>>1)|((Map226_Reg[0]&0x80)>>3)|((Map226_Reg[1]&0x01)<<5);
  
  if( Map226_Reg[0] & 0x20 ) {
    if( Map226_Reg[0] & 0x01 ) {
      ROMBANK0 = ROMPAGE(((bank_number<<2)+2) % (NesHeader.rom_size<<1));
      ROMBANK1 = ROMPAGE(((bank_number<<2)+3) % (NesHeader.rom_size<<1));
      ROMBANK2 = ROMPAGE(((bank_number<<2)+2) % (NesHeader.rom_size<<1));
      ROMBANK3 = ROMPAGE(((bank_number<<2)+3) % (NesHeader.rom_size<<1));
    } else {
      ROMBANK0 = ROMPAGE(((bank_number<<2)+0) % (NesHeader.rom_size<<1));
      ROMBANK1 = ROMPAGE(((bank_number<<2)+1) % (NesHeader.rom_size<<1));
      ROMBANK2 = ROMPAGE(((bank_number<<2)+0) % (NesHeader.rom_size<<1));
      ROMBANK3 = ROMPAGE(((bank_number<<2)+1) % (NesHeader.rom_size<<1));
    }
  } else {
    ROMBANK0 = ROMPAGE(((bank_number<<2)+0) % (NesHeader.rom_size<<1));
    ROMBANK1 = ROMPAGE(((bank_number<<2)+1) % (NesHeader.rom_size<<1));
    ROMBANK2 = ROMPAGE(((bank_number<<2)+2) % (NesHeader.rom_size<<1));
    ROMBANK3 = ROMPAGE(((bank_number<<2)+3) % (NesHeader.rom_size<<1));
  }
}
