/*===================================================================*/
/*                                                                   */
/*                     Mapper 51 : 11-in-1  映射器 51：11 合 1       */
/*                                                                   */
/*===================================================================*/

int     Map51_Mode, Map51_Bank;

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 51  初始化映射器 51                            */
/*-------------------------------------------------------------------*/
void Map51_Init()
{
  /* Initialize Mapper  初始化映射器 */
  MapperInit = Map51_Init;

  /* Write to Mapper  写入映射器 */
  MapperWrite = Map51_Write;

  /* Write to SRAM  写入 SRAM */
  MapperSram = Map51_Sram;

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

  /* Set Registers  设置寄存器 */
  Map51_Bank = 0;
  Map51_Mode = 1;

  /* Set ROM Banks  设置 ROM 银行 */
  Map51_Set_CPU_Banks();

  /* Set PPU Banks  设置 PPU 银行 */
  for ( int page_number = 0; page_number < 8; ++page_number )
    PPUBANK[ page_number ] = CRAMPAGE( page_number );
  InfoNES_SetupChr();

  /* Set up wiring of the interrupt pin  设置中断引脚接线方式 */
  K6502_Set_Int_Wiring( 1, 1 ); 
}

/*-------------------------------------------------------------------*/
/*  Mapper 51 Write Function  映射器 51：写函数                      */
/*-------------------------------------------------------------------*/
void Map51_Write( uint16_t address, uint8_t byte_data )
{
  Map51_Bank = (byte_data & 0x0f) << 2;
  if( 0xC000 <= address && address <= 0xDFFF ) {
    Map51_Mode = (Map51_Mode & 0x01) | ((byte_data & 0x10) >> 3);
  }
  Map51_Set_CPU_Banks();
}

/*-------------------------------------------------------------------*/
/*  Mapper 51 Write to SRAM Function  映射器 51：写 SRAM 函数        */
/*-------------------------------------------------------------------*/
void Map51_Sram( uint16_t address, uint8_t byte_data )
{
  if( address>=0x6000 ) {
    Map51_Mode = ((byte_data & 0x10) >> 3) | ((byte_data & 0x02) >> 1);
    Map51_Set_CPU_Banks();
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 51 Set CPU Banks Function  映射器 51：设置 CPU 银行函数   */
/*-------------------------------------------------------------------*/
void Map51_Set_CPU_Banks()
{
  switch(Map51_Mode) {
  case 0:
    InfoNES_Mirroring( 1 );
    SRAMBANK = ROMPAGE((Map51_Bank|0x2c|3) % (NesHeader.rom_size<<1));
    ROMBANK0 = ROMPAGE((Map51_Bank|0x00|0) % (NesHeader.rom_size<<1));
    ROMBANK1 = ROMPAGE((Map51_Bank|0x00|1) % (NesHeader.rom_size<<1));
    ROMBANK2 = ROMPAGE((Map51_Bank|0x0c|2) % (NesHeader.rom_size<<1));
    ROMBANK3 = ROMPAGE((Map51_Bank|0x0c|3) % (NesHeader.rom_size<<1));
    break;
  case 1:
    InfoNES_Mirroring( 1 );
    SRAMBANK = ROMPAGE((Map51_Bank|0x20|3) % (NesHeader.rom_size<<1));
    ROMBANK0 = ROMPAGE((Map51_Bank|0x00|0) % (NesHeader.rom_size<<1));
    ROMBANK1 = ROMPAGE((Map51_Bank|0x00|1) % (NesHeader.rom_size<<1));
    ROMBANK2 = ROMPAGE((Map51_Bank|0x00|2) % (NesHeader.rom_size<<1));
    ROMBANK3 = ROMPAGE((Map51_Bank|0x00|3) % (NesHeader.rom_size<<1));
    break;
  case 2:
    InfoNES_Mirroring( 1 );
    SRAMBANK = ROMPAGE((Map51_Bank|0x2e|3) % (NesHeader.rom_size<<1));
    ROMBANK0 = ROMPAGE((Map51_Bank|0x02|0) % (NesHeader.rom_size<<1));
    ROMBANK1 = ROMPAGE((Map51_Bank|0x02|1) % (NesHeader.rom_size<<1));
    ROMBANK2 = ROMPAGE((Map51_Bank|0x0e|2) % (NesHeader.rom_size<<1));
    ROMBANK3 = ROMPAGE((Map51_Bank|0x0e|3) % (NesHeader.rom_size<<1));
    break;
  case 3:
    InfoNES_Mirroring( 0 );
    SRAMBANK = ROMPAGE((Map51_Bank|0x20|3) % (NesHeader.rom_size<<1));
    ROMBANK0 = ROMPAGE((Map51_Bank|0x00|0) % (NesHeader.rom_size<<1));
    ROMBANK1 = ROMPAGE((Map51_Bank|0x00|1) % (NesHeader.rom_size<<1));
    ROMBANK2 = ROMPAGE((Map51_Bank|0x00|2) % (NesHeader.rom_size<<1));
    ROMBANK3 = ROMPAGE((Map51_Bank|0x00|3) % (NesHeader.rom_size<<1));
    break;
  }
}

