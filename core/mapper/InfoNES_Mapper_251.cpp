/*===================================================================*/
/*                                                                   */
/*                          Mapper 251  映射器 251                   */
/*                                                                   */
/*===================================================================*/

uint8_t	Map251_Reg[11];
uint8_t	Map251_Breg[4];

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 251  初始化映射器 251                          */
/*-------------------------------------------------------------------*/
void Map251_Init()
{
  /* Initialize Mapper  初始化映射器 */
  MapperInit = Map251_Init;

  /* Write to Mapper  写入映射器 */
  MapperWrite = Map251_Write;

  /* Write to SRAM  写入 SRAM */
  MapperSram = Map251_Sram;

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

  /* Set Registers  设置寄存器 */
  InfoNES_Mirroring( 1 );

  int	i;
  for( i = 0; i < 11; i++ )
    Map251_Reg[i] = 0;
  for( i = 0; i < 4; i++ )
    Map251_Breg[i] = 0;

  /* Set up wiring of the interrupt pin  设置中断引脚接线方式 */
  K6502_Set_Int_Wiring( 1, 1 ); 
}

/*-------------------------------------------------------------------*/
/*  Mapper 251 Write Function  映射器 251：写函数                    */
/*-------------------------------------------------------------------*/
void Map251_Write( uint16_t address, uint8_t byte_data )
{
  switch( address & 0xE001 ) {
  case	0x8000:
    Map251_Reg[8] = byte_data;
    Map251_Set_Banks();
    break;
  case	0x8001:
    Map251_Reg[Map251_Reg[8]&0x07] = byte_data;
    Map251_Set_Banks();
    break;
  case	0xA001:
    if( byte_data & 0x80 ) {
      Map251_Reg[ 9] = 1;
      Map251_Reg[10] = 0;
    } else {
      Map251_Reg[ 9] = 0;
    }
    break;
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 251 Write to SRAM Function  映射器 251：写 SRAM 函数      */
/*-------------------------------------------------------------------*/
void Map251_Sram( uint16_t address, uint8_t byte_data )
{
  if( (address & 0xE001) == 0x6000 ) {
    if( Map251_Reg[9] ) {
      Map251_Breg[Map251_Reg[10]++] = byte_data;
      if( Map251_Reg[10] == 4 ) {
	Map251_Reg[10] = 0;
	Map251_Set_Banks();
      }
    }
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 251 Set Banks Function  映射器 251：设置银行函数          */
/*-------------------------------------------------------------------*/
void Map251_Set_Banks()
{
  int	chr_pages[6];
  int	prg_pages[4];

  for( int i = 0; i < 6; i++ ) {
    chr_pages[i] = (Map251_Reg[i]|(Map251_Breg[1]<<4)) & ((Map251_Breg[2]<<4)|0x0F);
  }

  if( Map251_Reg[8] & 0x80 ) {
    PPUBANK[ 0 ] = VROMPAGE(chr_pages[2] % (NesHeader.v_rom_size<<3));
    PPUBANK[ 1 ] = VROMPAGE(chr_pages[3] % (NesHeader.v_rom_size<<3));
    PPUBANK[ 2 ] = VROMPAGE(chr_pages[4] % (NesHeader.v_rom_size<<3));
    PPUBANK[ 3 ] = VROMPAGE(chr_pages[5] % (NesHeader.v_rom_size<<3));
    PPUBANK[ 4 ] = VROMPAGE(chr_pages[0] % (NesHeader.v_rom_size<<3));
    PPUBANK[ 5 ] = VROMPAGE((chr_pages[0]+1) % (NesHeader.v_rom_size<<3));
    PPUBANK[ 6 ] = VROMPAGE(chr_pages[1] % (NesHeader.v_rom_size<<3));
    PPUBANK[ 7 ] = VROMPAGE((chr_pages[1]+1) % (NesHeader.v_rom_size<<3));
    InfoNES_SetupChr();
  } else {
    PPUBANK[ 0 ] = VROMPAGE(chr_pages[0] % (NesHeader.v_rom_size<<3));
    PPUBANK[ 1 ] = VROMPAGE((chr_pages[0]+1) % (NesHeader.v_rom_size<<3));
    PPUBANK[ 2 ] = VROMPAGE(chr_pages[1] % (NesHeader.v_rom_size<<3));
    PPUBANK[ 3 ] = VROMPAGE((chr_pages[1]+1) % (NesHeader.v_rom_size<<3));
    PPUBANK[ 4 ] = VROMPAGE(chr_pages[2] % (NesHeader.v_rom_size<<3));
    PPUBANK[ 5 ] = VROMPAGE(chr_pages[3] % (NesHeader.v_rom_size<<3));
    PPUBANK[ 6 ] = VROMPAGE(chr_pages[4] % (NesHeader.v_rom_size<<3));
    PPUBANK[ 7 ] = VROMPAGE(chr_pages[5] % (NesHeader.v_rom_size<<3));
    InfoNES_SetupChr();
  }

  prg_pages[0] = (Map251_Reg[6]&((Map251_Breg[3]&0x3F)^0x3F))|(Map251_Breg[1]);
  prg_pages[1] = (Map251_Reg[7]&((Map251_Breg[3]&0x3F)^0x3F))|(Map251_Breg[1]);
  prg_pages[2] = prg_pages[3] =((Map251_Breg[3]&0x3F)^0x3F)|(Map251_Breg[1]);
  prg_pages[2] &= (NesHeader.rom_size<<1)-1;

  if( Map251_Reg[8] & 0x40 ) {
    ROMBANK0 = ROMPAGE(prg_pages[2] % (NesHeader.rom_size<<1));
    ROMBANK1 = ROMPAGE(prg_pages[1] % (NesHeader.rom_size<<1));
    ROMBANK2 = ROMPAGE(prg_pages[0] % (NesHeader.rom_size<<1));
    ROMBANK3 = ROMPAGE(prg_pages[3] % (NesHeader.rom_size<<1));
  } else { 
    ROMBANK0 = ROMPAGE(prg_pages[0] % (NesHeader.rom_size<<1));
    ROMBANK1 = ROMPAGE(prg_pages[1] % (NesHeader.rom_size<<1));
    ROMBANK2 = ROMPAGE(prg_pages[2] % (NesHeader.rom_size<<1));
    ROMBANK3 = ROMPAGE(prg_pages[3] % (NesHeader.rom_size<<1));
  }
}
