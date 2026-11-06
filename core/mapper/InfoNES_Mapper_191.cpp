/*===================================================================*/
/*                                                                   */
/* Mapper 191 : SACHEN Super Cartridge Xin1 (Ver.1-9), Q-BOY Support  映射器 191：SACHEN 超级卡带 Xin1（1-9 版），支持 Q-BOY */
/*                                                                   */
/*===================================================================*/

uint8_t	Map191_Reg[8];
uint8_t	Map191_Prg0, Map191_Prg1;
uint8_t	Map191_Chr0, Map191_Chr1, Map191_Chr2, Map191_Chr3;
uint8_t	Map191_Highbank;

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 191  初始化映射器 191                          */
/*-------------------------------------------------------------------*/
void Map191_Init()
{
  /* Initialize Mapper  初始化映射器 */
  MapperInit = Map191_Init;

  /* Write to Mapper  写入映射器 */
  MapperWrite = Map0_Write;

  /* Write to SRAM  写入 SRAM */
  MapperSram = Map0_Sram;

  /* Write to APU  写入 APU */
  MapperApu = Map191_Apu;

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
  for( int i = 0; i < 8; i++ ) {
    Map191_Reg[i] = 0x00;
  }

  /* Set ROM Banks  设置 ROM 银行 */
  Map191_Prg0 = 0;
  // Map191_Prg1 = 1;
  Map191_Set_CPU_Banks();

  /* Set PPU Banks  设置 PPU 银行 */
  Map191_Chr0 = 0;
  Map191_Chr1 = 0;
  Map191_Chr2 = 0;
  Map191_Chr3 = 0;
  Map191_Highbank = 0;
  Map191_Set_PPU_Banks();

  /* Set up wiring of the interrupt pin  设置中断引脚接线方式 */
  K6502_Set_Int_Wiring( 1, 1 ); 
}

/*-------------------------------------------------------------------*/
/*  Mapper 191 Write to APU Function  映射器 191：写 APU 函数        */
/*-------------------------------------------------------------------*/
void Map191_Apu( uint16_t address, uint8_t byte_data )
{
  switch( address ) {
  case	0x4100:
    Map191_Reg[0]=byte_data;
    break;
  case	0x4101:
    Map191_Reg[1]=byte_data;
    switch( Map191_Reg[0] ) {
    case	0:
      Map191_Chr0=byte_data&7;
      Map191_Set_PPU_Banks();
      break;
    case	1:
      Map191_Chr1=byte_data&7;
      Map191_Set_PPU_Banks();
      break;
    case	2:
      Map191_Chr2=byte_data&7;
      Map191_Set_PPU_Banks();
      break;
    case	3:
      Map191_Chr3=byte_data&7;
      Map191_Set_PPU_Banks();
      break;
    case	4:
      Map191_Highbank=byte_data&7;
      Map191_Set_PPU_Banks();
      break;
    case	5:
      Map191_Prg0=byte_data&7;
      Map191_Set_CPU_Banks();
      break;
    case	7:
      if( byte_data & 0x02 ) InfoNES_Mirroring( 0 );
      else		  InfoNES_Mirroring( 1 );
      break;
    }
    break;
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 191 Set CPU Banks Function  映射器 191：设置 CPU 银行函数 */
/*-------------------------------------------------------------------*/
void	Map191_Set_CPU_Banks()
{
  ROMBANK0 = ROMPAGE( ((Map191_Prg0<<2) + 0 ) % ( NesHeader.rom_size << 1 ) );
  ROMBANK1 = ROMPAGE( ((Map191_Prg0<<2) + 1 ) % ( NesHeader.rom_size << 1 ) );
  ROMBANK2 = ROMPAGE( ((Map191_Prg0<<2) + 2 ) % ( NesHeader.rom_size << 1 ) );
  ROMBANK3 = ROMPAGE( ((Map191_Prg0<<2) + 3 ) % ( NesHeader.rom_size << 1 ) );
};

/*-------------------------------------------------------------------*/
/*  Mapper 191 Set PPU Banks Function  映射器 191：设置 PPU 银行函数 */
/*-------------------------------------------------------------------*/
void	Map191_Set_PPU_Banks()
{
  if ( NesHeader.v_rom_size > 0 ) {
    PPUBANK[ 0 ] = VROMPAGE( ((((Map191_Highbank<<3)+Map191_Chr0)<<2)+0 ) % ( NesHeader.v_rom_size << 3 ) );
    PPUBANK[ 1 ] = VROMPAGE( ((((Map191_Highbank<<3)+Map191_Chr0)<<2)+1 ) % ( NesHeader.v_rom_size << 3 ) );
    PPUBANK[ 2 ] = VROMPAGE( ((((Map191_Highbank<<3)+Map191_Chr1)<<2)+2 ) % ( NesHeader.v_rom_size << 3 ) );
    PPUBANK[ 3 ] = VROMPAGE( ((((Map191_Highbank<<3)+Map191_Chr1)<<2)+3 ) % ( NesHeader.v_rom_size << 3 ) );
    PPUBANK[ 4 ] = VROMPAGE( ((((Map191_Highbank<<3)+Map191_Chr2)<<2)+0 ) % ( NesHeader.v_rom_size << 3 ) );
    PPUBANK[ 5 ] = VROMPAGE( ((((Map191_Highbank<<3)+Map191_Chr2)<<2)+1 ) % ( NesHeader.v_rom_size << 3 ) );
    PPUBANK[ 6 ] = VROMPAGE( ((((Map191_Highbank<<3)+Map191_Chr3)<<2)+2 ) % ( NesHeader.v_rom_size << 3 ) );
    PPUBANK[ 7 ] = VROMPAGE( ((((Map191_Highbank<<3)+Map191_Chr3)<<2)+3 ) % ( NesHeader.v_rom_size << 3 ) );
    InfoNES_SetupChr();
  }
}
