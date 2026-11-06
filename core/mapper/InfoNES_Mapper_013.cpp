/*===================================================================*/
/*                                                                   */
/*                        Mapper 13 : CPROM  映射器 13：CPROM        */
/*                                                                   */
/*===================================================================*/

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 13  初始化映射器 13                            */
/*-------------------------------------------------------------------*/
void Map13_Init()
{
  /* Initialize Mapper  初始化映射器 */
  MapperInit = Map13_Init;

  /* Write to Mapper  写入映射器 */
  MapperWrite = Map13_Write;

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

  /* Set PPU Banks  设置 PPU 银行 */
  PPUBANK[ 0 ] = CRAMPAGE( 0 );
  PPUBANK[ 1 ] = CRAMPAGE( 1 );
  PPUBANK[ 2 ] = CRAMPAGE( 2 );
  PPUBANK[ 3 ] = CRAMPAGE( 3 );
  PPUBANK[ 4 ] = CRAMPAGE( 0 );
  PPUBANK[ 5 ] = CRAMPAGE( 1 );
  PPUBANK[ 6 ] = CRAMPAGE( 2 );
  PPUBANK[ 7 ] = CRAMPAGE( 3 );
  InfoNES_SetupChr();

  /* Set up wiring of the interrupt pin  设置中断引脚接线方式 */
  K6502_Set_Int_Wiring( 1, 1 ); 
}

/*-------------------------------------------------------------------*/
/*  Mapper 13 Write Function  映射器 13：写函数                      */
/*-------------------------------------------------------------------*/
void Map13_Write( uint16_t address, uint8_t byte_data )
{
  /* Set ROM Banks  设置 ROM 银行 */
  ROMBANK0 = ROMPAGE((((byte_data&0x30)>>2)+0) % (NesHeader.rom_size<<1));
  ROMBANK1 = ROMPAGE((((byte_data&0x30)>>2)+1) % (NesHeader.rom_size<<1));
  ROMBANK2 = ROMPAGE((((byte_data&0x30)>>2)+2) % (NesHeader.rom_size<<1));
  ROMBANK3 = ROMPAGE((((byte_data&0x30)>>2)+3) % (NesHeader.rom_size<<1));

  /* Set PPU Banks  设置 PPU 银行 */
  PPUBANK[ 4 ] = CRAMPAGE(((byte_data&0x03)<<2)+0);
  PPUBANK[ 5 ] = CRAMPAGE(((byte_data&0x03)<<2)+1);
  PPUBANK[ 6 ] = CRAMPAGE(((byte_data&0x03)<<2)+2);
  PPUBANK[ 7 ] = CRAMPAGE(((byte_data&0x03)<<2)+3);
  InfoNES_SetupChr();
}
