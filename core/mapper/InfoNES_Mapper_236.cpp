/*===================================================================*/
/*                                                                   */
/*                   Mapper 236 : 800-in-1  映射器 236：800 合 1     */
/*                                                                   */
/*===================================================================*/

uint8_t    Map236_Bank, Map236_Mode;

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 236  初始化映射器 236                          */
/*-------------------------------------------------------------------*/
void Map236_Init()
{
  /* Initialize Mapper  初始化映射器 */
  MapperInit = Map236_Init;

  /* Write to Mapper  写入映射器 */
  MapperWrite = Map236_Write;

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
  ROMBANK2 = ROMLASTPAGE( 1 );
  ROMBANK3 = ROMLASTPAGE( 0 );

  /* Set Registers  设置寄存器 */
  Map236_Bank = Map236_Mode = 0;

  /* Set up wiring of the interrupt pin  设置中断引脚接线方式 */
  K6502_Set_Int_Wiring( 1, 1 ); 
}

/*-------------------------------------------------------------------*/
/*  Mapper 236 Write Function  映射器 236：写函数                    */
/*-------------------------------------------------------------------*/
void Map236_Write( uint16_t address, uint8_t byte_data )
{
  if( address >= 0x8000 && address <= 0xBFFF ) {
    Map236_Bank = ((address&0x03)<<4)|(Map236_Bank&0x07);
  } else {
    Map236_Bank = (address&0x07)|(Map236_Bank&0x30);
    Map236_Mode = address&0x30;
  }

  if( address & 0x20 ) {
    InfoNES_Mirroring( 0 );
  } else {
    InfoNES_Mirroring( 1 );
  }

  switch( Map236_Mode ) {
  case	0x00:
    Map236_Bank |= 0x08;
    ROMBANK0 = ROMPAGE(((Map236_Bank<<1)+0) % (NesHeader.rom_size<<1));
    ROMBANK1 = ROMPAGE(((Map236_Bank<<1)+1) % (NesHeader.rom_size<<1));
    ROMBANK2 = ROMPAGE((((Map236_Bank|0x07)<<1)+0) % (NesHeader.rom_size<<1));
    ROMBANK3 = ROMPAGE((((Map236_Bank|0x07)<<1)+1) % (NesHeader.rom_size<<1));
    break;
  case	0x10:
    Map236_Bank |= 0x37;
    ROMBANK0 = ROMPAGE(((Map236_Bank<<1)+0) % (NesHeader.rom_size<<1));
    ROMBANK1 = ROMPAGE(((Map236_Bank<<1)+1) % (NesHeader.rom_size<<1));
    ROMBANK2 = ROMPAGE((((Map236_Bank|0x07)<<1)+0) % (NesHeader.rom_size<<1));
    ROMBANK3 = ROMPAGE((((Map236_Bank|0x07)<<1)+1) % (NesHeader.rom_size<<1));
    break;
  case	0x20:
    Map236_Bank |= 0x08;
    ROMBANK0 = ROMPAGE((((Map236_Bank&0xFE)<<1)+0) % (NesHeader.rom_size<<1));
    ROMBANK1 = ROMPAGE((((Map236_Bank&0xFE)<<1)+1) % (NesHeader.rom_size<<1));
    ROMBANK2 = ROMPAGE((((Map236_Bank&0xFE)<<1)+2) % (NesHeader.rom_size<<1));
    ROMBANK3 = ROMPAGE((((Map236_Bank&0xFE)<<1)+3) % (NesHeader.rom_size<<1));
    break;
  case	0x30:
    Map236_Bank |= 0x08;
    ROMBANK0 = ROMPAGE(((Map236_Bank<<1)+0) % (NesHeader.rom_size<<1));
    ROMBANK1 = ROMPAGE(((Map236_Bank<<1)+1) % (NesHeader.rom_size<<1));
    ROMBANK2 = ROMPAGE(((Map236_Bank<<1)+0) % (NesHeader.rom_size<<1));
    ROMBANK3 = ROMPAGE(((Map236_Bank<<1)+1) % (NesHeader.rom_size<<1));
    break;
  }
}
