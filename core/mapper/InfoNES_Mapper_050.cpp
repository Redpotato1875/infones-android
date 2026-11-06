/*===================================================================*/
/*                                                                   */
/*                        Mapper 50 (Pirates)  映射器 50（Pirates）  */
/*                                                                   */
/*===================================================================*/

uint8_t Map50_IRQ_Enable;

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 50  初始化映射器 50                            */
/*-------------------------------------------------------------------*/
void Map50_Init()
{
  /* Initialize Mapper  初始化映射器 */
  MapperInit = Map50_Init;

  /* Write to Mapper  写入映射器 */
  MapperWrite = Map0_Write;

  /* Write to SRAM  写入 SRAM */
  MapperSram = Map0_Sram;

  /* Write to APU  写入 APU */
  MapperApu = Map50_Apu;

  /* Read from APU  从 APU 读取 */
  MapperReadApu = Map0_ReadApu;

  /* Callback at VSync  场同步回调 */
  MapperVSync = Map0_VSync;

  /* Callback at HSync  行同步回调 */
  MapperHSync = Map50_HSync;

  /* Callback at PPU  PPU 回调 */
  MapperPPU = Map0_PPU;

  /* Callback at Rendering Screen ( 1:BG, 0:Sprite )  渲染画面回调（1：背景，0：精灵） */
  MapperRenderScreen = Map0_RenderScreen;

  /* Set SRAM Banks  设置 SRAM 银行 */
  SRAMBANK = ROMPAGE( 15 % ( NesHeader.rom_size << 1 ) );

  /* Set ROM Banks  设置 ROM 银行 */
  ROMBANK0 = ROMPAGE( 8 % ( NesHeader.rom_size << 1 ) );
  ROMBANK1 = ROMPAGE( 9 % ( NesHeader.rom_size << 1 ) );
  ROMBANK2 = ROMPAGE( 0 % ( NesHeader.rom_size << 1 ) );
  ROMBANK3 = ROMPAGE( 11 % ( NesHeader.rom_size << 1 ) );

  /* Set PPU Banks  设置 PPU 银行 */
  if ( NesHeader.v_rom_size > 0 )
  {
    for ( int page_number = 0; page_number < 8; ++page_number )
    {
      PPUBANK[ page_number ] = VROMPAGE( page_number );
    }    
    InfoNES_SetupChr();
  }

  /* Initialize IRQ Registers  初始化 IRQ 寄存器 */
  Map50_IRQ_Enable = 0;

  /* Set up wiring of the interrupt pin  设置中断引脚接线方式 */
  K6502_Set_Int_Wiring( 1, 1 ); 
}

/*-------------------------------------------------------------------*/
/*  Mapper 50 Write to Apu Function  映射器 50：写 APU 函数          */
/*-------------------------------------------------------------------*/
void Map50_Apu( uint16_t address, uint8_t byte_data )
{
  if ( ( address & 0xE060 ) == 0x4020 )
  {
    if( address & 0x0100 )
    {
      Map50_IRQ_Enable = byte_data & 0x01;
    }
    else
    {
      uint8_t dummy;

      dummy = ( byte_data & 0x08 ) | ( ( byte_data & 0x01 ) << 2 ) | ( ( byte_data & 0x06 ) >> 1 );
      ROMBANK2 = ROMPAGE( dummy % ( NesHeader.rom_size << 1 ) );
    }
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 50 H-Sync Function  映射器 50：行同步函数                 */
/*-------------------------------------------------------------------*/
void Map50_HSync()
{
/*
 *  Callback at HSync  行同步回调
 *
 */
  if ( Map50_IRQ_Enable )
  {
    if ( PPU_Scanline == 21 )
    {
      IRQ_REQ;
    }
  }
}
