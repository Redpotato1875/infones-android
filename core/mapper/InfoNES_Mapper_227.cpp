/*===================================================================*/
/*                                                                   */
/*                    Mapper 227 : 1200-in-1  映射器 227：1200 合 1  */
/*                                                                   */
/*===================================================================*/

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 227  初始化映射器 227                          */
/*-------------------------------------------------------------------*/
void Map227_Init()
{
  /* Initialize Mapper  初始化映射器 */
  MapperInit = Map227_Init;

  /* Write to Mapper  写入映射器 */
  MapperWrite = Map227_Write;

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

  /* Set up wiring of the interrupt pin  设置中断引脚接线方式 */
  K6502_Set_Int_Wiring( 1, 1 ); 
}

/*-------------------------------------------------------------------*/
/*  Mapper 227 Write Function  映射器 227：写函数                    */
/*-------------------------------------------------------------------*/
void Map227_Write( uint16_t address, uint8_t byte_data )
{
  uint8_t	bank_number = ((address&0x0100)>>4)|((address&0x0078)>>3);

  if( address & 0x0001 ) {
    ROMBANK0 = ROMPAGE(((bank_number<<2)+0) % (NesHeader.rom_size<<1));
    ROMBANK1 = ROMPAGE(((bank_number<<2)+1) % (NesHeader.rom_size<<1));
    ROMBANK2 = ROMPAGE(((bank_number<<2)+2) % (NesHeader.rom_size<<1));
    ROMBANK3 = ROMPAGE(((bank_number<<2)+3) % (NesHeader.rom_size<<1));
  } else {
    if( address & 0x0004 ) {
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
  }
  
  if( !(address & 0x0080) ) {
    if( address & 0x0200 ) {
      ROMBANK2 = ROMPAGE((((bank_number&0x1C)<<2)+14) % (NesHeader.rom_size<<1));
      ROMBANK3 = ROMPAGE((((bank_number&0x1C)<<2)+15) % (NesHeader.rom_size<<1));
    } else {
      ROMBANK2 = ROMPAGE((((bank_number&0x1C)<<2)+0) % (NesHeader.rom_size<<1));
      ROMBANK3 = ROMPAGE((((bank_number&0x1C)<<2)+1) % (NesHeader.rom_size<<1));
    }
  }
  if( address & 0x0002 ) {
    InfoNES_Mirroring( 0 );
  } else {
    InfoNES_Mirroring( 1 );
  }
}
