/*===================================================================*/
/*                                                                   */
/*                         Mapper 185  (Tecmo)  映射器 185（Tecmo）  */
/*                                                                   */
/*===================================================================*/

uint8_t Map185_Dummy_Chr_Rom[ 0x400 ];

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 185  初始化映射器 185                          */
/*-------------------------------------------------------------------*/
void Map185_Init()
{
  /* Initialize Mapper  初始化映射器 */
  MapperInit = Map185_Init;

  /* Write to Mapper  写入映射器 */
  MapperWrite = Map185_Write;

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

  /* Initialize Dummy VROM  初始化虚拟 VROM */
  for ( int page_number = 0; page_number < 0x400; page_number++ )
  {
    Map185_Dummy_Chr_Rom[ page_number ] = 0xff;
  }

  /* Set up wiring of the interrupt pin  设置中断引脚接线方式 */
  K6502_Set_Int_Wiring( 1, 1 ); 
}

/*-------------------------------------------------------------------*/
/*  Mapper 185 Write Function  映射器 185：写函数                    */
/*-------------------------------------------------------------------*/
void Map185_Write( uint16_t address, uint8_t byte_data )
{
  /* Set PPU Banks  设置 PPU 银行 */
  if ( byte_data & 0x03 )
  {
    PPUBANK[ 0 ] = VROMPAGE( 0 );
    PPUBANK[ 1 ] = VROMPAGE( 1 );
    PPUBANK[ 2 ] = VROMPAGE( 2 );
    PPUBANK[ 3 ] = VROMPAGE( 3 );
    PPUBANK[ 4 ] = VROMPAGE( 4 );
    PPUBANK[ 5 ] = VROMPAGE( 5 );
    PPUBANK[ 6 ] = VROMPAGE( 6 );
    PPUBANK[ 7 ] = VROMPAGE( 7 );
    InfoNES_SetupChr();
  } else {
    PPUBANK[ 0 ] = Map185_Dummy_Chr_Rom;
    PPUBANK[ 1 ] = Map185_Dummy_Chr_Rom;
    PPUBANK[ 2 ] = Map185_Dummy_Chr_Rom;
    PPUBANK[ 3 ] = Map185_Dummy_Chr_Rom;
    PPUBANK[ 4 ] = Map185_Dummy_Chr_Rom;
    PPUBANK[ 5 ] = Map185_Dummy_Chr_Rom;
    PPUBANK[ 6 ] = Map185_Dummy_Chr_Rom;
    PPUBANK[ 7 ] = Map185_Dummy_Chr_Rom;
    InfoNES_SetupChr();
  }
}
