/*===================================================================*/
/*                                                                   */
/*                            Mapper 108  映射器 108                 */
/*                                                                   */
/*===================================================================*/

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 108  初始化映射器 108                          */
/*-------------------------------------------------------------------*/
void Map108_Init()
{
  /* Initialize Mapper  初始化映射器 */
  MapperInit = Map108_Init;

  /* Write to Mapper  写入映射器 */
  MapperWrite = Map108_Write;

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
  SRAMBANK = ROMPAGE( 0 );

  /* Set ROM Banks  设置 ROM 银行 */
  ROMBANK0 = ROMPAGE( 0xC % ( NesHeader.rom_size << 1 ) );
  ROMBANK1 = ROMPAGE( 0xD % ( NesHeader.rom_size << 1 ) );
  ROMBANK2 = ROMPAGE( 0xE % ( NesHeader.rom_size << 1 ) );
  ROMBANK3 = ROMPAGE( 0xF % ( NesHeader.rom_size << 1 ) );

  /* Set up wiring of the interrupt pin  设置中断引脚接线方式 */
  K6502_Set_Int_Wiring( 1, 1 ); 
}

/*-------------------------------------------------------------------*/
/*  Mapper 108 Write Function  映射器 108：写函数                    */
/*-------------------------------------------------------------------*/
void Map108_Write( uint16_t address, uint8_t byte_data )
{
  /* Set SRAM Banks  设置 SRAM 银行 */
  SRAMBANK = ROMPAGE( byte_data % ( NesHeader.rom_size << 1 ) );
}
