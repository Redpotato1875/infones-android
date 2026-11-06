/*===================================================================*/
/*                                                                   */
/*                     Mapper 255 : 110-in-1  映射器 255：110 合 1   */
/*                                                                   */
/*===================================================================*/

uint8_t    Map255_Reg[4];

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 255  初始化映射器 255                          */
/*-------------------------------------------------------------------*/
void Map255_Init()
{
  /* Initialize Mapper  初始化映射器 */
  MapperInit = Map255_Init;

  /* Write to Mapper  写入映射器 */
  MapperWrite = Map255_Write;

  /* Write to SRAM  写入 SRAM */
  MapperSram = Map0_Sram;

  /* Write to APU  写入 APU */
  MapperApu = Map255_Apu;

  /* Read from APU  从 APU 读取 */
  MapperReadApu = Map255_ReadApu;

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

  /* Set PPU Banks  设置 PPU 银行 */
  if ( NesHeader.v_rom_size > 0 ) {
    for ( int page_number = 0; page_number < 8; ++page_number )
      PPUBANK[ page_number ] = VROMPAGE( page_number );
    InfoNES_SetupChr();
  }

  /* Set Registers  设置寄存器 */
  InfoNES_Mirroring( 1 );

  Map255_Reg[0] = 0;
  Map255_Reg[1] = 0;
  Map255_Reg[2] = 0;
  Map255_Reg[3] = 0;

  /* Set up wiring of the interrupt pin  设置中断引脚接线方式 */
  K6502_Set_Int_Wiring( 1, 1 ); 
}

/*-------------------------------------------------------------------*/
/*  Mapper 255 Write Function  映射器 255：写函数                    */
/*-------------------------------------------------------------------*/
void Map255_Write( uint16_t address, uint8_t byte_data )
{
  uint8_t	prg = (address & 0x0F80)>>7;
  int	chr_pages = (address & 0x003F);
  int	bank_select = (address & 0x4000)>>14;

  if( address & 0x2000 ) {
    InfoNES_Mirroring( 0 );
  } else {
    InfoNES_Mirroring( 1 );
  }

  if( address & 0x1000 ) {
    if( address & 0x0040 ) {
      ROMBANK0 = ROMPAGE((0x80*bank_select+prg*4+2) % (NesHeader.rom_size<<1));
      ROMBANK1 = ROMPAGE((0x80*bank_select+prg*4+3) % (NesHeader.rom_size<<1));
      ROMBANK2 = ROMPAGE((0x80*bank_select+prg*4+2) % (NesHeader.rom_size<<1));
      ROMBANK3 = ROMPAGE((0x80*bank_select+prg*4+3) % (NesHeader.rom_size<<1));
    } else {
      ROMBANK0 = ROMPAGE((0x80*bank_select+prg*4+0) % (NesHeader.rom_size<<1));
      ROMBANK1 = ROMPAGE((0x80*bank_select+prg*4+1) % (NesHeader.rom_size<<1));
      ROMBANK2 = ROMPAGE((0x80*bank_select+prg*4+0) % (NesHeader.rom_size<<1));
      ROMBANK3 = ROMPAGE((0x80*bank_select+prg*4+1) % (NesHeader.rom_size<<1));
    }
  } else {
    ROMBANK0 = ROMPAGE((0x80*bank_select+prg*4+0) % (NesHeader.rom_size<<1));
    ROMBANK1 = ROMPAGE((0x80*bank_select+prg*4+1) % (NesHeader.rom_size<<1));
    ROMBANK2 = ROMPAGE((0x80*bank_select+prg*4+2) % (NesHeader.rom_size<<1));
    ROMBANK3 = ROMPAGE((0x80*bank_select+prg*4+3) % (NesHeader.rom_size<<1));
  }
  
  PPUBANK[ 0 ] = VROMPAGE((0x200*bank_select+chr_pages*8+0) % (NesHeader.v_rom_size<<3));
  PPUBANK[ 1 ] = VROMPAGE((0x200*bank_select+chr_pages*8+1) % (NesHeader.v_rom_size<<3));
  PPUBANK[ 2 ] = VROMPAGE((0x200*bank_select+chr_pages*8+2) % (NesHeader.v_rom_size<<3));
  PPUBANK[ 3 ] = VROMPAGE((0x200*bank_select+chr_pages*8+3) % (NesHeader.v_rom_size<<3));
  PPUBANK[ 4 ] = VROMPAGE((0x200*bank_select+chr_pages*8+4) % (NesHeader.v_rom_size<<3));
  PPUBANK[ 5 ] = VROMPAGE((0x200*bank_select+chr_pages*8+5) % (NesHeader.v_rom_size<<3));
  PPUBANK[ 6 ] = VROMPAGE((0x200*bank_select+chr_pages*8+6) % (NesHeader.v_rom_size<<3));
  PPUBANK[ 7 ] = VROMPAGE((0x200*bank_select+chr_pages*8+7) % (NesHeader.v_rom_size<<3));
}

/*-------------------------------------------------------------------*/
/*  Mapper 255 Write to APU Function  映射器 255：写 APU 函数        */
/*-------------------------------------------------------------------*/
void Map255_Apu( uint16_t address, uint8_t byte_data )
{
  if( address >= 0x5800 ) {
    Map255_Reg[address&0x0003] = byte_data & 0x0F;
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 255 Read from APU Function  映射器 255：读 APU 函数       */
/*-------------------------------------------------------------------*/
uint8_t Map255_ReadApu( uint16_t address )
{
  if( address >= 0x5800 ) {
    return	Map255_Reg[address&0x0003] & 0x0F;
  } else {
    return	address>>8;
  }
}
