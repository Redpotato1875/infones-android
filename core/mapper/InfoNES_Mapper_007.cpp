/*===================================================================*/
/*                                                                   */
/*                        Mapper 7 (AOROM)  映射器 7（AOROM）        */
/*                                                                   */
/*===================================================================*/

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 7  初始化映射器 7                              */
/*-------------------------------------------------------------------*/
void Map7_Init()
{
  /* Initialize Mapper  初始化映射器 */
  MapperInit = Map7_Init;

  /* Write to Mapper  写入映射器 */
  MapperWrite = Map7_Write;

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
/*  Mapper 7 Write Function  映射器 7：写函数                        */
/*-------------------------------------------------------------------*/
void Map7_Write( uint16_t address, uint8_t byte_data )
{
  uint8_t bank_number;

  /* Set ROM Banks  设置 ROM 银行 */
  bank_number = ( byte_data & 0x07 ) << 2;
  bank_number %= ( NesHeader.rom_size << 1 );

  ROMBANK0 = ROMPAGE( bank_number );
  ROMBANK1 = ROMPAGE( bank_number + 1 );
  ROMBANK2 = ROMPAGE( bank_number + 2 );
  ROMBANK3 = ROMPAGE( bank_number + 3 );

  /* Name Table Mirroring  命名表镜像 */
  InfoNES_Mirroring( byte_data & 0x10 ? 2 : 3 );
}
