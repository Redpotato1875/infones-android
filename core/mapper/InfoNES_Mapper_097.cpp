/*===================================================================*/
/*                                                                   */
/*                  Mapper 97 (74161/32 Irem)  映射器 97（74161/32 Irem） */
/*                                                                   */
/*===================================================================*/

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 97  初始化映射器 97                            */
/*-------------------------------------------------------------------*/
void Map97_Init()
{
  /* Initialize Mapper  初始化映射器 */
  MapperInit = Map97_Init;

  /* Write to Mapper  写入映射器 */
  MapperWrite = Map97_Write;

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
  ROMBANK0 = ROMLASTPAGE( 1 );
  ROMBANK1 = ROMLASTPAGE( 0 );
  ROMBANK2 = ROMPAGE( 0 );
  ROMBANK3 = ROMPAGE( 1 );

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
/*  Mapper 97 Write Function  映射器 97：写函数                      */
/*-------------------------------------------------------------------*/
void Map97_Write( uint16_t address, uint8_t byte_data )
{
  /* Set ROM Banks  设置 ROM 银行 */
  if ( address < 0xc000 )
  {
    uint8_t prg_bank = byte_data & 0x0f;

    prg_bank <<= 1;
    prg_bank %= ( NesHeader.rom_size << 1 );

    ROMBANK2 = ROMPAGE( prg_bank );
    ROMBANK3 = ROMPAGE( prg_bank + 1 );

    if ( ( byte_data & 0x80 ) == 0 )
    {
      InfoNES_Mirroring( 0 );
    } else {
      InfoNES_Mirroring( 1 );
    }
  }
}
