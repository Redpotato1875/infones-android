/*===================================================================*/
/*                                                                   */
/*                   Mapper 135 : SACHEN CHEN  映射器 135：SACHEN CHEN */
/*                                                                   */
/*===================================================================*/

uint8_t    Map135_Cmd;
uint8_t	Map135_Chr0l, Map135_Chr1l, Map135_Chr0h, Map135_Chr1h, Map135_Chrch;

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 135  初始化映射器 135                          */
/*-------------------------------------------------------------------*/
void Map135_Init()
{
  /* Initialize Mapper  初始化映射器 */
  MapperInit = Map135_Init;

  /* Write to Mapper  写入映射器 */
  MapperWrite = Map0_Write;

  /* Write to SRAM  写入 SRAM */
  MapperSram = Map0_Sram;

  /* Write to APU  写入 APU */
  MapperApu = Map135_Apu;

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
  Map135_Cmd = 0;
  Map135_Chr0l = Map135_Chr1l = Map135_Chr0h = Map135_Chr1h = Map135_Chrch = 0;

  /* Set ROM Banks  设置 ROM 银行 */
  ROMBANK0 = ROMPAGE( 0 );
  ROMBANK1 = ROMPAGE( 1 );
  ROMBANK2 = ROMPAGE( 2 );
  ROMBANK3 = ROMPAGE( 3 );

  /* Set PPU Banks  设置 PPU 银行 */
  Map135_Set_PPU_Banks();

  /* Set up wiring of the interrupt pin  设置中断引脚接线方式 */
  K6502_Set_Int_Wiring( 1, 1 ); 
}

/*-------------------------------------------------------------------*/
/*  Mapper 135 Write to APU Function  映射器 135：写 APU 函数        */
/*-------------------------------------------------------------------*/
void Map135_Apu( uint16_t address, uint8_t byte_data )
{
  switch( address & 0x4101 ) {
  case	0x4100:
    Map135_Cmd = byte_data & 0x07;
    break;
  case	0x4101:
    switch( Map135_Cmd ) {
    case	0:
      Map135_Chr0l = byte_data & 0x07;
      Map135_Set_PPU_Banks();
      break;
    case	1:
      Map135_Chr0h = byte_data & 0x07;
      Map135_Set_PPU_Banks();
      break;
    case	2:
      Map135_Chr1l = byte_data & 0x07;
      Map135_Set_PPU_Banks();
      break;
    case	3:
      Map135_Chr1h = byte_data & 0x07;
      Map135_Set_PPU_Banks();
      break;
    case	4:
      Map135_Chrch = byte_data & 0x07;
      Map135_Set_PPU_Banks();
      break;
    case	5:
      /* Set ROM Banks  设置 ROM 银行 */
      ROMBANK0 = ROMPAGE( (((byte_data%0x07)<<2) + 0 ) % (NesHeader.rom_size << 1) );
      ROMBANK1 = ROMPAGE( (((byte_data%0x07)<<2) + 1 ) % (NesHeader.rom_size << 1) );
      ROMBANK2 = ROMPAGE( (((byte_data%0x07)<<2) + 2 ) % (NesHeader.rom_size << 1) );
      ROMBANK3 = ROMPAGE( (((byte_data%0x07)<<2) + 3 ) % (NesHeader.rom_size << 1) );
      break;
    case	6:
      break;
    case	7:
      switch( (byte_data>>1)&0x03 ) {
      case	0: InfoNES_Mirroring( 2 ); break;
      case	1: InfoNES_Mirroring( 0  ); break;
      case	2: InfoNES_Mirroring( 1  ); break;
      case	3: InfoNES_Mirroring( 2 ); break;
      }
      break;
    }
    break;
  }
  //Map135_Wram[ address & 0x1fff ] = byte_data;
}

/*-------------------------------------------------------------------*/
/*  Mapper 135 Set PPU Banks Function  映射器 135：设置 PPU 银行函数 */
/*-------------------------------------------------------------------*/
void	Map135_Set_PPU_Banks()
{
  /* Set PPU Banks  设置 PPU 银行 */
  PPUBANK[ 0 ] = VROMPAGE( (((0|(Map135_Chr0l<<1)|(Map135_Chrch<<4))<<1) + 0) % (NesHeader.v_rom_size << 3) );
  PPUBANK[ 1 ] = VROMPAGE( (((0|(Map135_Chr0l<<1)|(Map135_Chrch<<4))<<1) + 1) % (NesHeader.v_rom_size << 3) );
  PPUBANK[ 2 ] = VROMPAGE( (((1|(Map135_Chr0h<<1)|(Map135_Chrch<<4))<<1) + 0) % (NesHeader.v_rom_size << 3) );
  PPUBANK[ 3 ] = VROMPAGE( (((1|(Map135_Chr0h<<1)|(Map135_Chrch<<4))<<1) + 1) % (NesHeader.v_rom_size << 3) );
  PPUBANK[ 4 ] = VROMPAGE( (((0|(Map135_Chr1l<<1)|(Map135_Chrch<<4))<<1) + 0) % (NesHeader.v_rom_size << 3) );
  PPUBANK[ 5 ] = VROMPAGE( (((0|(Map135_Chr1l<<1)|(Map135_Chrch<<4))<<1) + 1) % (NesHeader.v_rom_size << 3) );
  PPUBANK[ 6 ] = VROMPAGE( (((1|(Map135_Chr1h<<1)|(Map135_Chrch<<4))<<1) + 0) % (NesHeader.v_rom_size << 3) );
  PPUBANK[ 7 ] = VROMPAGE( (((1|(Map135_Chr1h<<1)|(Map135_Chrch<<4))<<1) + 1) % (NesHeader.v_rom_size << 3) );
  InfoNES_SetupChr();
}
