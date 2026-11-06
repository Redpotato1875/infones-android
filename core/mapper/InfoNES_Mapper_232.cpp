/*===================================================================*/
/*                                                                   */
/*                   Mapper 232 : Quattro Games  映射器 232：Quattro Games */
/*                                                                   */
/*===================================================================*/

uint8_t Map232_Regs[2];

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 232  初始化映射器 232                          */
/*-------------------------------------------------------------------*/
void Map232_Init()
{
  /* Initialize Mapper  初始化映射器 */
  MapperInit = Map232_Init;

  /* Write to Mapper  写入映射器 */
  MapperWrite = Map232_Write;

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

  /* Initialize Registers  初始化寄存器 */
  Map232_Regs[0] = 0x0C;
  Map232_Regs[1] = 0x00;
  
  /* Set up wiring of the interrupt pin  设置中断引脚接线方式 */
  K6502_Set_Int_Wiring( 1, 1 ); 
}

/*-------------------------------------------------------------------*/
/*  Mapper 232 Write Function  映射器 232：写函数                    */
/*-------------------------------------------------------------------*/
void Map232_Write( uint16_t address, uint8_t byte_data )
{
  if( address == 0x9000 ) {
    Map232_Regs[0] = (byte_data & 0x18)>>1;
  } else if( address >= 0xA000 && address <= 0xFFFF ) {
    Map232_Regs[1] = byte_data & 0x03;
  }
  
  ROMBANK0= ROMPAGE((((Map232_Regs[0]|Map232_Regs[1])<<1)+0) % (NesHeader.rom_size<<1));
  ROMBANK1= ROMPAGE((((Map232_Regs[0]|Map232_Regs[1])<<1)+1) % (NesHeader.rom_size<<1));
  ROMBANK2= ROMPAGE((((Map232_Regs[0]|0x03)<<1)+0) % (NesHeader.rom_size<<1));
  ROMBANK3= ROMPAGE((((Map232_Regs[0]|0x03)<<1)+1) % (NesHeader.rom_size<<1));
}
