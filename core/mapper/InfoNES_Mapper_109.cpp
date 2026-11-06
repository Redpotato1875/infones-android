/*===================================================================*/
/*                                                                   */
/*          Mapper 109 : SACHEN The Great Wall SA-019  映射器 109：SACHEN 长城 SA-019 */
/*                                                                   */
/*===================================================================*/

uint8_t	Map109_Reg;
uint8_t	Map109_Chr0, Map109_Chr1, Map109_Chr2, Map109_Chr3;
uint8_t	Map109_Chrmode0, Map109_Chrmode1;

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 109  初始化映射器 109                          */
/*-------------------------------------------------------------------*/
void Map109_Init()
{
  /* Initialize Mapper  初始化映射器 */
  MapperInit = Map109_Init;

  /* Write to Mapper  写入映射器 */
  MapperWrite = Map0_Write;

  /* Write to SRAM  写入 SRAM */
  MapperSram = Map0_Sram;

  /* Write to APU  写入 APU */
  MapperApu = Map109_Apu;

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

  /* Initialize Registers  初始化寄存器 */
  Map109_Reg = 0;
  Map109_Chr0 = 0;
  Map109_Chr1 = 0;
  Map109_Chr2 = 0;
  Map109_Chr3 = 0;
  Map109_Chrmode0 = 0;
  Map109_Chrmode1 = 0;

  /* Set PPU Banks  设置 PPU 银行 */
  Map109_Set_PPU_Banks();

  /* Set up wiring of the interrupt pin  设置中断引脚接线方式 */
  K6502_Set_Int_Wiring( 1, 1 ); 
}

/*-------------------------------------------------------------------*/
/*  Mapper 109 Write to APU Function  映射器 109：写 APU 函数        */
/*-------------------------------------------------------------------*/
void Map109_Apu( uint16_t address, uint8_t byte_data )
{
  switch( address ) {
  case	0x4100:
    Map109_Reg = byte_data;
    break;
  case	0x4101:
    switch( Map109_Reg ) {
    case 0:
      Map109_Chr0 = byte_data;
      Map109_Set_PPU_Banks();
      break;
    case 1:
      Map109_Chr1 = byte_data;
      Map109_Set_PPU_Banks();
      break;
    case 2:
      Map109_Chr2 = byte_data;
      Map109_Set_PPU_Banks();
      break;
    case 3:
      Map109_Chr3 = byte_data;
      Map109_Set_PPU_Banks();
      break;
    case 4:
      Map109_Chrmode0 = byte_data & 0x01;
      Map109_Set_PPU_Banks();
      break;
    case 5:
      ROMBANK0 = ROMPAGE( ( ( byte_data & 0x07 ) + 0 ) % ( NesHeader.rom_size << 1 ) );
      ROMBANK1 = ROMPAGE( ( ( byte_data & 0x07 ) + 1 ) % ( NesHeader.rom_size << 1 ) );
      ROMBANK2 = ROMPAGE( ( ( byte_data & 0x07 ) + 2 ) % ( NesHeader.rom_size << 1 ) );
      ROMBANK3 = ROMPAGE( ( ( byte_data & 0x07 ) + 3 ) % ( NesHeader.rom_size << 1 ) );
      break;
    case 6:
      Map109_Chrmode1 = byte_data & 0x07;
      Map109_Set_PPU_Banks();
      break;
    case 7:
      if( byte_data & 0x01 ) InfoNES_Mirroring( 0 );
      else		  InfoNES_Mirroring( 1 );
      break;
    }
    break;
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 109 Set Bank PPU Function  映射器 109：设置 PPU 银行函数  */
/*-------------------------------------------------------------------*/
void Map109_Set_PPU_Banks()
{
  if ( NesHeader.v_rom_size > 0 ) {
    PPUBANK[ 0 ] = VROMPAGE((Map109_Chr0) % (NesHeader.v_rom_size<<3) );
    PPUBANK[ 1 ] = VROMPAGE((Map109_Chr1|((Map109_Chrmode1<<3)&0x8)) % (NesHeader.v_rom_size<<3));
    PPUBANK[ 2 ] = VROMPAGE((Map109_Chr2|((Map109_Chrmode1<<2)&0x8)) % (NesHeader.v_rom_size<<3));
    PPUBANK[ 3 ] = VROMPAGE((Map109_Chr3|((Map109_Chrmode1<<1)&0x8)|(Map109_Chrmode0*0x10)) % (NesHeader.v_rom_size<<3));
    PPUBANK[ 4 ] = VROMPAGE((NesHeader.v_rom_size<<3)-4);
    PPUBANK[ 5 ] = VROMPAGE((NesHeader.v_rom_size<<3)-3);
    PPUBANK[ 6 ] = VROMPAGE((NesHeader.v_rom_size<<3)-2);
    PPUBANK[ 7 ] = VROMPAGE((NesHeader.v_rom_size<<3)-1);
    InfoNES_SetupChr();
  }
}
