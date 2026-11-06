/*===================================================================*/
/*                                                                   */
/*                        Mapper 91 (Pirates)  映射器 91（Pirates）  */
/*                                                                   */
/*===================================================================*/

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 91  初始化映射器 91                            */
/*-------------------------------------------------------------------*/
void Map91_Init()
{
  /* Initialize Mapper  初始化映射器 */
  MapperInit = Map91_Init;

  /* Write to Mapper  写入映射器 */
  MapperWrite = Map0_Write;

  /* Write to SRAM  写入 SRAM */
  MapperSram = Map91_Sram;

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
  ROMBANK2 = ROMLASTPAGE( 1 );
  ROMBANK3 = ROMLASTPAGE( 0 );

  /* Set PPU Banks  设置 PPU 银行 */
  if ( NesHeader.v_rom_size > 0 )
  {
    for ( int page_number = 0; page_number < 8; ++page_number )
      PPUBANK[ page_number ] = VROMPAGE( page_number );
    InfoNES_SetupChr();
  }

  /* Set Name Table Mirroring  设置命名表镜像 */
  InfoNES_Mirroring( 1 );

  /* Set up wiring of the interrupt pin  设置中断引脚接线方式 */
  K6502_Set_Int_Wiring( 1, 1 ); 
}

/*-------------------------------------------------------------------*/
/*  Mapper 91 Write to Sram Function  映射器 91：写 SRAM 函数        */
/*-------------------------------------------------------------------*/
void Map91_Sram( uint16_t address, uint8_t byte_data )
{
  switch( address & 0xF00F)
  {
    /* Set PPU Banks  设置 PPU 银行 */
    case 0x6000:
      PPUBANK[ 0 ] = VROMPAGE( (byte_data*2+0) % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 1 ] = VROMPAGE( (byte_data*2+1) % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();
      break;

    case 0x6001:
      PPUBANK[ 2 ] = VROMPAGE( (byte_data*2+0) % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 3 ] = VROMPAGE( (byte_data*2+1) % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();
      break;

    case 0x6002:
      PPUBANK[ 4 ] = VROMPAGE( (byte_data*2+0) % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 5 ] = VROMPAGE( (byte_data*2+1) % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();
      break;

    case 0x6003:
      PPUBANK[ 6 ] = VROMPAGE( (byte_data*2+0) % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 7 ] = VROMPAGE( (byte_data*2+1) % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();
      break;

    /* Set CPU Banks  设置 CPU 银行 */
    case 0x7000:
      ROMBANK0 = ROMPAGE( byte_data % ( NesHeader.rom_size << 1 ) ); 
      break;

    case 0x7001:
      ROMBANK1 = ROMPAGE( byte_data % ( NesHeader.rom_size << 1 ) ); 
      break;
  }
}
