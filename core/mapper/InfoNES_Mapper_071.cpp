/*===================================================================*/
/*                                                                   */
/*            Mapper 71 (Camerica Custom Mapper)  映射器 71（Camerica Custom Mapper） */
/*                                                                   */
/*===================================================================*/

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 71  初始化映射器 71                            */
/*-------------------------------------------------------------------*/
void Map71_Init()
{
  /* Initialize Mapper  初始化映射器 */
  MapperInit = Map71_Init;

  /* Write to Mapper  写入映射器 */
  MapperWrite = Map71_Write;

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

  /* Set up wiring of the interrupt pin  设置中断引脚接线方式 */
  K6502_Set_Int_Wiring( 1, 1 ); 
}

/*-------------------------------------------------------------------*/
/*  Mapper 71 Write Function  映射器 71：写函数                      */
/*-------------------------------------------------------------------*/
void Map71_Write( uint16_t address, uint8_t byte_data )
{
  switch ( address & 0xf000 )
  {
    case 0x9000:
      if ( byte_data & 0x10 )
      {
        InfoNES_Mirroring( 2 );
      } else {
        InfoNES_Mirroring( 3 );
      }
      break;

    /* Set ROM Banks  设置 ROM 银行 */
    case 0xc000:
    case 0xd000:
    case 0xe000:
    case 0xf000:
      ROMBANK0 = ROMPAGE( ( ( byte_data << 1 ) + 0 ) % ( NesHeader.rom_size << 1 ) );
      ROMBANK1 = ROMPAGE( ( ( byte_data << 1 ) + 1 ) % ( NesHeader.rom_size << 1 ) );
      break;
  }
}
