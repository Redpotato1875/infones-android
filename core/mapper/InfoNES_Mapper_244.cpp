/*===================================================================*/
/*                                                                   */
/*                          Mapper 244  映射器 244                   */
/*                                                                   */
/*===================================================================*/

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 244  初始化映射器 244                          */
/*-------------------------------------------------------------------*/
void Map244_Init()
{
  /* Initialize Mapper  初始化映射器 */
  MapperInit = Map244_Init;

  /* Write to Mapper  写入映射器 */
  MapperWrite = Map244_Write;

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

  /* Set up wiring of the interrupt pin  设置中断引脚接线方式 */
  K6502_Set_Int_Wiring( 1, 1 ); 
}

/*-------------------------------------------------------------------*/
/*  Mapper 244 Write Function  映射器 244：写函数                    */
/*-------------------------------------------------------------------*/
void Map244_Write( uint16_t address, uint8_t byte_data )
{
  if( address>=0x8065 && address<=0x80A4 ) {
    /* Set ROM Banks  设置 ROM 银行 */
    ROMBANK0 = ROMPAGE(((((address-0x8065)&0x3)<<2)+0) % (NesHeader.rom_size<<1));
    ROMBANK1 = ROMPAGE(((((address-0x8065)&0x3)<<2)+1) % (NesHeader.rom_size<<1));
    ROMBANK2 = ROMPAGE(((((address-0x8065)&0x3)<<2)+2) % (NesHeader.rom_size<<1));
    ROMBANK3 = ROMPAGE(((((address-0x8065)&0x3)<<2)+3) % (NesHeader.rom_size<<1));
  }
  
  if( address>=0x80A5 && address<=0x80E4 ) {
    /* Set ROM Banks  设置 ROM 银行 */
    ROMBANK0 = ROMPAGE(((((address-0x80A5)&0x7)<<2)+0) % (NesHeader.rom_size<<1));
    ROMBANK1 = ROMPAGE(((((address-0x80A5)&0x7)<<2)+1) % (NesHeader.rom_size<<1));
    ROMBANK2 = ROMPAGE(((((address-0x80A5)&0x7)<<2)+2) % (NesHeader.rom_size<<1));
    ROMBANK3 = ROMPAGE(((((address-0x80A5)&0x7)<<2)+3) % (NesHeader.rom_size<<1));
  }
}
