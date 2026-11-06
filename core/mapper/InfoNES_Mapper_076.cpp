/*===================================================================*/
/*                                                                   */
/*                    Mapper 76 (Namcot 109)  映射器 76（Namcot 109） */
/*                                                                   */
/*===================================================================*/

uint8_t Map76_Reg;

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 76  初始化映射器 76                            */
/*-------------------------------------------------------------------*/
void Map76_Init()
{
  /* Initialize Mapper  初始化映射器 */
  MapperInit = Map76_Init;

  /* Write to Mapper  写入映射器 */
  MapperWrite = Map76_Write;

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
/*  Mapper 76 Write Function  映射器 76：写函数                      */
/*-------------------------------------------------------------------*/
void Map76_Write( uint16_t address, uint8_t byte_data )
{
  switch ( address )
  {
    case 0x8000:
      Map76_Reg = byte_data;
      break;

    case 0x8001:
      switch ( Map76_Reg & 0x07 )
      {
        case 0x02:
          byte_data <<= 1;
          byte_data %= ( NesHeader.v_rom_size << 3 );
          PPUBANK[ 0 ] = VROMPAGE( byte_data );
          PPUBANK[ 1 ] = VROMPAGE( byte_data + 1 );
          InfoNES_SetupChr();
          break;

        case 0x03:
          byte_data <<= 1;
          byte_data %= ( NesHeader.v_rom_size << 3 );
          PPUBANK[ 2 ] = VROMPAGE( byte_data );
          PPUBANK[ 3 ] = VROMPAGE( byte_data + 1 );
          InfoNES_SetupChr();
          break;

        case 0x04:
          byte_data <<= 1;
          byte_data %= ( NesHeader.v_rom_size << 3 );
          PPUBANK[ 4 ] = VROMPAGE( byte_data );
          PPUBANK[ 5 ] = VROMPAGE( byte_data + 1 );
          InfoNES_SetupChr();
          break;

        case 0x05:
          byte_data <<= 1;
          byte_data %= ( NesHeader.v_rom_size << 3 );
          PPUBANK[ 6 ] = VROMPAGE( byte_data );
          PPUBANK[ 7 ] = VROMPAGE( byte_data + 1 );
          InfoNES_SetupChr();
          break;

        case 0x06:
          byte_data %= ( NesHeader.rom_size << 1 );
          ROMBANK0 = ROMPAGE( byte_data );
          break;

        case 0x07:
          byte_data %= ( NesHeader.rom_size << 1 );
          ROMBANK1 = ROMPAGE( byte_data );
          break;
      }
      break;
  }  
}
