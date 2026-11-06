/*===================================================================*/
/*                                                                   */
/*                Mapper 77  (Irem Early Mapper #0)  映射器 77（Irem 早期映射器 #0） */
/*                                                                   */
/*===================================================================*/

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 77  初始化映射器 77                            */
/*-------------------------------------------------------------------*/
void Map77_Init()
{
  /* Initialize Mapper  初始化映射器 */
  MapperInit = Map77_Init;

  /* Write to Mapper  写入映射器 */
  MapperWrite = Map77_Write;

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

  /* VRAM Write Enabled  VRAM 写使能 */
  vram_write_enable = 1;

  /* Set up wiring of the interrupt pin  设置中断引脚接线方式 */
  K6502_Set_Int_Wiring( 1, 1 ); 
}

/*-------------------------------------------------------------------*/
/*  Mapper 77 Write Function  映射器 77：写函数                      */
/*-------------------------------------------------------------------*/
void Map77_Write( uint16_t address, uint8_t byte_data )
{
  uint8_t rom_bank = byte_data & 0x07;
  uint8_t chr_bank = ( byte_data & 0xf0 ) >> 4;

  /* Set ROM Banks  设置 ROM 银行 */
  rom_bank <<= 2;
  rom_bank %= ( NesHeader.rom_size << 1 );

  ROMBANK0 = ROMPAGE( rom_bank );
  ROMBANK1 = ROMPAGE( rom_bank + 1 );
  ROMBANK2 = ROMPAGE( rom_bank + 2 );
  ROMBANK3 = ROMPAGE( rom_bank + 3 );

  /* Set PPU Banks  设置 PPU 银行 */
  chr_bank <<= 1;
  chr_bank %= ( NesHeader.v_rom_size << 3 );

  PPUBANK[ 0 ] = VROMPAGE( chr_bank );
  PPUBANK[ 1 ] = VROMPAGE( chr_bank + 1 );
  InfoNES_SetupChr();
}
