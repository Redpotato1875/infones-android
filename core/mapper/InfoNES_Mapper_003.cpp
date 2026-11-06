/*===================================================================*/
/*                                                                   */
/*                     Mapper 3 (VROM Switch)  映射器 3（VROM Switch） */
/*                                                                   */
/*===================================================================*/

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 3  初始化映射器 3                              */
/*-------------------------------------------------------------------*/
void Map3_Init()
{
  int page_number;

  /* Initialize Mapper  初始化映射器 */
  MapperInit = Map3_Init;

  /* Write to Mapper  写入映射器 */
  MapperWrite = Map3_Write;

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
  if ( ( NesHeader.rom_size << 1 ) > 2 )
  {
    ROMBANK0 = ROMPAGE( 0 );
    ROMBANK1 = ROMPAGE( 1 );
    ROMBANK2 = ROMPAGE( 2 );
    ROMBANK3 = ROMPAGE( 3 );    
  } else {
    ROMBANK0 = ROMPAGE( 0 );
    ROMBANK1 = ROMPAGE( 1 );
    ROMBANK2 = ROMPAGE( 0 );
    ROMBANK3 = ROMPAGE( 1 );
  }

  /* Set PPU Banks  设置 PPU 银行 */
  if ( NesHeader.v_rom_size > 0 )
  {
    for ( page_number = 0; page_number < 8; ++page_number )
    {
      PPUBANK[ page_number ] = VROMPAGE( page_number );
    }
    InfoNES_SetupChr();
  }

  /* Set up wiring of the interrupt pin  设置中断引脚接线方式 */
  /* "DragonQuest" doesn't run if IRQ isn't made to occur in CLI  “DragonQuest”需要让 IRQ 在 CLI 下发生才能运行 */
  K6502_Set_Int_Wiring( 1, 1 ); 
}

/*-------------------------------------------------------------------*/
/*  Mapper 3 Write Function  映射器 3：写函数                        */
/*-------------------------------------------------------------------*/
void Map3_Write( uint16_t address, uint8_t byte_data )
{
  uint32_t base_address;

  /* Set PPU Banks  设置 PPU 银行 */
  byte_data %= NesHeader.v_rom_size;
  base_address = ( (uint32_t)byte_data ) << 3;

  PPUBANK[ 0 ] = VROMPAGE( base_address + 0 );
  PPUBANK[ 1 ] = VROMPAGE( base_address + 1 );
  PPUBANK[ 2 ] = VROMPAGE( base_address + 2 );
  PPUBANK[ 3 ] = VROMPAGE( base_address + 3 );
  PPUBANK[ 4 ] = VROMPAGE( base_address + 4 );
  PPUBANK[ 5 ] = VROMPAGE( base_address + 5 );
  PPUBANK[ 6 ] = VROMPAGE( base_address + 6 );
  PPUBANK[ 7 ] = VROMPAGE( base_address + 7 );

  InfoNES_SetupChr();
}
