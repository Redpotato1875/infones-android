/*===================================================================*/
/*                                                                   */
/*                   Mapper 46 (Color Dreams)  映射器 46（Color Dreams） */
/*                                                                   */
/*===================================================================*/

uint8_t Map46_Regs[ 4 ];

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 46  初始化映射器 46                            */
/*-------------------------------------------------------------------*/
void Map46_Init()
{
  /* Initialize Mapper  初始化映射器 */
  MapperInit = Map46_Init;

  /* Write to Mapper  写入映射器 */
  MapperWrite = Map46_Write;

  /* Write to SRAM  写入 SRAM */
  MapperSram = Map46_Sram;

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
  Map46_Regs[ 0 ] = Map46_Regs[ 1 ] = Map46_Regs[ 2 ] = Map46_Regs[ 3 ] = 0;
  Map46_Set_ROM_Banks();

  /* Name Table Mirroring  命名表镜像 */
  InfoNES_Mirroring( 1 );
  
  /* Set up wiring of the interrupt pin  设置中断引脚接线方式 */
  K6502_Set_Int_Wiring( 1, 1 ); 
}

/*-------------------------------------------------------------------*/
/*  Mapper 46 Write to SRAM Function  映射器 46：写 SRAM 函数        */
/*-------------------------------------------------------------------*/
void Map46_Sram( uint16_t address, uint8_t byte_data )
{
  /* Set ROM Banks  设置 ROM 银行 */
  Map46_Regs[ 0 ] = byte_data & 0x0f;
  Map46_Regs[ 1 ] = ( byte_data & 0xf0 ) >> 4;
  Map46_Set_ROM_Banks();
}

/*-------------------------------------------------------------------*/
/*  Mapper 46 Write Function  映射器 46：写函数                      */
/*-------------------------------------------------------------------*/
void Map46_Write( uint16_t address, uint8_t byte_data )
{
  /* Set ROM Banks  设置 ROM 银行 */
  Map46_Regs[ 2 ] = byte_data & 0x01;
  Map46_Regs[ 3 ] = ( byte_data & 0x70 ) >> 4;
  Map46_Set_ROM_Banks();
}

/*-------------------------------------------------------------------*/
/*  Mapper 46 Setup ROM Banks Function  映射器 46：设置 ROM 银行函数 */
/*-------------------------------------------------------------------*/
void Map46_Set_ROM_Banks()
{
  /* Set ROM Banks  设置 ROM 银行 */
  ROMBANK0 = ROMPAGE( ( ( Map46_Regs[ 0 ] << 3 ) + ( Map46_Regs[ 2 ] << 2 ) + 0 ) % ( NesHeader.rom_size << 1 ) );  
  ROMBANK1 = ROMPAGE( ( ( Map46_Regs[ 0 ] << 3 ) + ( Map46_Regs[ 2 ] << 2 ) + 1 ) % ( NesHeader.rom_size << 1 ) );
  ROMBANK2 = ROMPAGE( ( ( Map46_Regs[ 0 ] << 3 ) + ( Map46_Regs[ 2 ] << 2 ) + 2 ) % ( NesHeader.rom_size << 1 ) );
  ROMBANK3 = ROMPAGE( ( ( Map46_Regs[ 0 ] << 3 ) + ( Map46_Regs[ 2 ] << 2 ) + 3 ) % ( NesHeader.rom_size << 1 ) ); 

  /* Set PPU Banks  设置 PPU 银行 */
  PPUBANK[ 0 ] = VROMPAGE( ( ( Map46_Regs[ 1 ] << 6 ) + ( Map46_Regs[ 3 ] << 3 ) + 0 ) % ( NesHeader.v_rom_size << 3 ) ); 
  PPUBANK[ 1 ] = VROMPAGE( ( ( Map46_Regs[ 1 ] << 6 ) + ( Map46_Regs[ 3 ] << 3 ) + 1 ) % ( NesHeader.v_rom_size << 3 ) ); 
  PPUBANK[ 2 ] = VROMPAGE( ( ( Map46_Regs[ 1 ] << 6 ) + ( Map46_Regs[ 3 ] << 3 ) + 2 ) % ( NesHeader.v_rom_size << 3 ) ); 
  PPUBANK[ 3 ] = VROMPAGE( ( ( Map46_Regs[ 1 ] << 6 ) + ( Map46_Regs[ 3 ] << 3 ) + 3 ) % ( NesHeader.v_rom_size << 3 ) ); 
  PPUBANK[ 4 ] = VROMPAGE( ( ( Map46_Regs[ 1 ] << 6 ) + ( Map46_Regs[ 3 ] << 3 ) + 4 ) % ( NesHeader.v_rom_size << 3 ) ); 
  PPUBANK[ 5 ] = VROMPAGE( ( ( Map46_Regs[ 1 ] << 6 ) + ( Map46_Regs[ 3 ] << 3 ) + 5 ) % ( NesHeader.v_rom_size << 3 ) ); 
  PPUBANK[ 6 ] = VROMPAGE( ( ( Map46_Regs[ 1 ] << 6 ) + ( Map46_Regs[ 3 ] << 3 ) + 6 ) % ( NesHeader.v_rom_size << 3 ) ); 
  PPUBANK[ 7 ] = VROMPAGE( ( ( Map46_Regs[ 1 ] << 6 ) + ( Map46_Regs[ 3 ] << 3 ) + 7 ) % ( NesHeader.v_rom_size << 3 ) ); 
  InfoNES_SetupChr();
}
