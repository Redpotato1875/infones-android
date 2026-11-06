/*===================================================================*/
/*                                                                   */
/*                 Mapper 70 (74161/32 Bandai)  映射器 70（74161/32 Bandai） */
/*                                                                   */
/*===================================================================*/

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 70  初始化映射器 70                            */
/*-------------------------------------------------------------------*/
void Map70_Init()
{
  /* Initialize Mapper  初始化映射器 */
  MapperInit = Map70_Init;

  /* Write to Mapper  写入映射器 */
  MapperWrite = Map70_Write;

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
/*  Mapper 70 Write Function  映射器 70：写函数                      */
/*-------------------------------------------------------------------*/
void Map70_Write( uint16_t address, uint8_t byte_data )
{
  uint8_t chr_bank = byte_data & 0x0f;
  uint8_t prg_bank = ( byte_data & 0x70 ) >> 4;

  /* Set ROM Banks  设置 ROM 银行 */
  prg_bank <<= 1;
  prg_bank %= ( NesHeader.rom_size << 1 );

  ROMBANK0 = ROMPAGE( prg_bank );
  ROMBANK1 = ROMPAGE( prg_bank + 1 );

  /* Set PPU Banks  设置 PPU 银行 */
  chr_bank <<= 3;
  chr_bank %= ( NesHeader.v_rom_size << 3 );

  PPUBANK[ 0 ] = VROMPAGE( chr_bank + 0 );
  PPUBANK[ 1 ] = VROMPAGE( chr_bank + 1 );
  PPUBANK[ 2 ] = VROMPAGE( chr_bank + 2 );
  PPUBANK[ 3 ] = VROMPAGE( chr_bank + 3 );
  PPUBANK[ 4 ] = VROMPAGE( chr_bank + 4 );
  PPUBANK[ 5 ] = VROMPAGE( chr_bank + 5 );
  PPUBANK[ 6 ] = VROMPAGE( chr_bank + 6 );
  PPUBANK[ 7 ] = VROMPAGE( chr_bank + 7 );
  InfoNES_SetupChr();

  /* Name Table Mirroring  命名表镜像 */
  if ( byte_data & 0x80 )
  {
    InfoNES_Mirroring( 2 );
  } else {
    InfoNES_Mirroring( 3 );
  }
}
