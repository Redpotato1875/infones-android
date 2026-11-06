/*===================================================================*/
/*                                                                   */
/*                      Mapper 122 (Sunsoft)  映射器 122（Sunsoft）  */
/*                                                                   */
/*===================================================================*/

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 122  初始化映射器 122                          */
/*-------------------------------------------------------------------*/
void Map122_Init()
{
  /* Initialize Mapper  初始化映射器 */
  MapperInit = Map122_Init;

  /* Write to Mapper  写入映射器 */
  MapperWrite = Map0_Write;

  /* Write to SRAM  写入 SRAM */
  MapperSram = Map122_Sram;

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
/*  Mapper 122 Write to Sram Function  映射器 122：写 SRAM 函数      */
/*-------------------------------------------------------------------*/
void Map122_Sram( uint16_t address, uint8_t byte_data )
{
  if ( address == 0x6000 )
  {
    uint8_t chr_bank0 = byte_data & 0x07;
    uint8_t chr_bank1 = ( byte_data & 0x70 ) >> 4;

    chr_bank0 = ( chr_bank0 << 2 ) % ( NesHeader.v_rom_size << 3 );
    chr_bank1 = ( chr_bank1 << 2 ) % ( NesHeader.v_rom_size << 3 );

    PPUBANK[ 0 ] = VROMPAGE( chr_bank0 + 0 );
    PPUBANK[ 1 ] = VROMPAGE( chr_bank0 + 1 );
    PPUBANK[ 2 ] = VROMPAGE( chr_bank0 + 2 );
    PPUBANK[ 3 ] = VROMPAGE( chr_bank0 + 3 );
    PPUBANK[ 4 ] = VROMPAGE( chr_bank1 + 0 );
    PPUBANK[ 5 ] = VROMPAGE( chr_bank1 + 1 );
    PPUBANK[ 6 ] = VROMPAGE( chr_bank1 + 2 );
    PPUBANK[ 7 ] = VROMPAGE( chr_bank1 + 3 );
    InfoNES_SetupChr();
  }
}
