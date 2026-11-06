/*===================================================================*/
/*                                                                   */
/*                   Mapper 96 : Bandai 74161  映射器 96：万代 74161 */
/*                                                                   */
/*===================================================================*/

uint8_t	Map96_Reg[2];

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 96  初始化映射器 96                            */
/*-------------------------------------------------------------------*/
void Map96_Init()
{
  /* Initialize Mapper  初始化映射器 */
  MapperInit = Map96_Init;

  /* Write to Mapper  写入映射器 */
  MapperWrite = Map96_Write;

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
  MapperPPU = Map96_PPU;

  /* Callback at Rendering Screen ( 1:BG, 0:Sprite )  渲染画面回调（1：背景，0：精灵） */
  MapperRenderScreen = Map0_RenderScreen;

  /* Set SRAM Banks  设置 SRAM 银行 */
  SRAMBANK = SRAM;

  /* Set Registers  设置寄存器 */
  Map96_Reg[0] = Map96_Reg[1] = 0;

  /* Set ROM Banks  设置 ROM 银行 */
  ROMBANK0 = ROMPAGE( 0 );
  ROMBANK1 = ROMPAGE( 1 );
  ROMBANK2 = ROMPAGE( 2 );
  ROMBANK3 = ROMPAGE( 3 );

  /* Set PPU Banks  设置 PPU 银行 */
  Map96_Set_Banks();
  InfoNES_Mirroring( 3 );

  /* Set up wiring of the interrupt pin  设置中断引脚接线方式 */
  K6502_Set_Int_Wiring( 1, 1 ); 
}

/*-------------------------------------------------------------------*/
/*  Mapper 96 Write Function  映射器 96：写函数                      */
/*-------------------------------------------------------------------*/
void Map96_Write( uint16_t address, uint8_t byte_data )
{
  ROMBANK0 = ROMPAGE((((byte_data & 0x03)<<2)+0) % (NesHeader.rom_size<<1));
  ROMBANK1 = ROMPAGE((((byte_data & 0x03)<<2)+1) % (NesHeader.rom_size<<1));
  ROMBANK2 = ROMPAGE((((byte_data & 0x03)<<2)+2) % (NesHeader.rom_size<<1));
  ROMBANK3 = ROMPAGE((((byte_data & 0x03)<<2)+3) % (NesHeader.rom_size<<1));
  
  Map96_Reg[0] = (byte_data & 0x04) >> 2;
  Map96_Set_Banks();
}

/*-------------------------------------------------------------------*/
/*  Mapper 96 PPU Function  映射器 96：PPU 函数                      */
/*-------------------------------------------------------------------*/
void Map96_PPU( uint16_t address )
{
  if( (address & 0xF000) == 0x2000 ) {
    Map96_Reg[1] = (address>>8)&0x03;
    Map96_Set_Banks();
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 96 Set Banks Function  映射器 96：设置银行函数            */
/*-------------------------------------------------------------------*/
void Map96_Set_Banks()
{
  PPUBANK[ 0 ] = CRAMPAGE(((Map96_Reg[0]*4+Map96_Reg[1])<<2)+0);
  PPUBANK[ 1 ] = CRAMPAGE(((Map96_Reg[0]*4+Map96_Reg[1])<<2)+1);
  PPUBANK[ 2 ] = CRAMPAGE(((Map96_Reg[0]*4+Map96_Reg[1])<<2)+2);
  PPUBANK[ 3 ] = CRAMPAGE(((Map96_Reg[0]*4+Map96_Reg[1])<<2)+3);
  PPUBANK[ 4 ] = CRAMPAGE(((Map96_Reg[0]*4+0x03)<<2)+0);
  PPUBANK[ 5 ] = CRAMPAGE(((Map96_Reg[0]*4+0x03)<<2)+1);
  PPUBANK[ 6 ] = CRAMPAGE(((Map96_Reg[0]*4+0x03)<<2)+2);
  PPUBANK[ 7 ] = CRAMPAGE(((Map96_Reg[0]*4+0x03)<<2)+3);
  InfoNES_SetupChr();
}

