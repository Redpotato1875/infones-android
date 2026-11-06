/*===================================================================*/
/*                                                                   */
/*                      Mapper 93 (74161/32)  映射器 93（74161/32）  */
/*                                                                   */
/*===================================================================*/

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 93  初始化映射器 93                            */
/*-------------------------------------------------------------------*/
void Map93_Init()
{
  /* Initialize Mapper  初始化映射器 */
  MapperInit = Map93_Init;

  /* Write to Mapper  写入映射器 */
  MapperWrite = Map0_Write;

  /* Write to SRAM  写入 SRAM */
  MapperSram = Map93_Sram;

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

  /* Set PPU Banks  设置 PPU 银行 */
  if ( NesHeader.v_rom_size > 0 )
  {
    for ( int page_number = 0; page_number < 8; ++page_number )
      PPUBANK[ page_number ] = VROMPAGE( page_number );
    InfoNES_SetupChr();
  }

  /* Set up wiring of the interrupt pin  设置中断引脚接线方式 */
  K6502_Set_Int_Wiring( 1, 1 ); 
}

/*-------------------------------------------------------------------*/
/*  Mapper 93 Write to Sram Function  映射器 93：写 SRAM 函数        */
/*-------------------------------------------------------------------*/
void Map93_Sram( uint16_t address, uint8_t byte_data )
{
  /* Set ROM Banks  设置 ROM 银行 */
  if ( address == 0x6000 )
  {
    byte_data <<= 1;
    byte_data %= ( NesHeader.rom_size << 1 );
  
    ROMBANK0 = ROMPAGE( byte_data );
    ROMBANK1 = ROMPAGE( byte_data + 1 );
  }
}
