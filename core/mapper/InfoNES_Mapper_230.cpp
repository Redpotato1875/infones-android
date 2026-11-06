/*===================================================================*/
/*                                                                   */
/*                       Mapper 230 : 22-in-1  映射器 230：22 合 1   */
/*                                                                   */
/*===================================================================*/

uint8_t Map230_RomSw;

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 230  初始化映射器 230                          */
/*-------------------------------------------------------------------*/
void Map230_Init()
{
  /* Initialize Mapper  初始化映射器 */
  MapperInit = Map230_Init;

  /* Write to Mapper  写入映射器 */
  MapperWrite = Map230_Write;

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

  /* Initialize Registers  初始化寄存器 */
  if( Map230_RomSw ) {
    Map230_RomSw = 0;
  } else {
    Map230_RomSw = 1;
  }

  /* Set ROM Banks  设置 ROM 银行 */
  if( Map230_RomSw ) {
    ROMBANK0 = ROMPAGE( 0 );
    ROMBANK1 = ROMPAGE( 1 );
    ROMBANK2 = ROMPAGE( 14 );
    ROMBANK3 = ROMPAGE( 15 );
  } else {
    ROMBANK0 = ROMPAGE( 16 );
    ROMBANK1 = ROMPAGE( 17 );
    ROMBANK2 = ROMLASTPAGE( 1 );
    ROMBANK3 = ROMLASTPAGE( 0 );
  }

  /* Set up wiring of the interrupt pin  设置中断引脚接线方式 */
  K6502_Set_Int_Wiring( 1, 1 ); 
}

/*-------------------------------------------------------------------*/
/*  Mapper 230 Write Function  映射器 230：写函数                    */
/*-------------------------------------------------------------------*/
void Map230_Write( uint16_t address, uint8_t byte_data )
{
  if( Map230_RomSw ) {
    ROMBANK0 = ROMPAGE((((byte_data&0x07)<<1)+0) % (NesHeader.rom_size<<1));
    ROMBANK1 = ROMPAGE((((byte_data&0x07)<<1)+1) % (NesHeader.rom_size<<1));
  } else {
    if( byte_data & 0x20 ) {
      ROMBANK0 = ROMPAGE((((byte_data&0x1F)<<1)+16) % (NesHeader.rom_size<<1));
      ROMBANK1 = ROMPAGE((((byte_data&0x1F)<<1)+17) % (NesHeader.rom_size<<1));
      ROMBANK2 = ROMPAGE((((byte_data&0x1F)<<1)+16) % (NesHeader.rom_size<<1));
      ROMBANK3 = ROMPAGE((((byte_data&0x1F)<<1)+17) % (NesHeader.rom_size<<1));
    } else {
      ROMBANK0 = ROMPAGE((((byte_data&0x1E)<<1)+16) % (NesHeader.rom_size<<1));
      ROMBANK1 = ROMPAGE((((byte_data&0x1E)<<1)+17) % (NesHeader.rom_size<<1));
      ROMBANK2 = ROMPAGE((((byte_data&0x1E)<<1)+18) % (NesHeader.rom_size<<1));
      ROMBANK3 = ROMPAGE((((byte_data&0x1E)<<1)+19) % (NesHeader.rom_size<<1));
    }
    if( byte_data & 0x40 ) {
      InfoNES_Mirroring( 1 );
    } else {
      InfoNES_Mirroring( 0 );
    }
  }
}
