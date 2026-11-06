/*===================================================================*/
/*                                                                   */
/*                     Mapper 235 : 150-in-1  映射器 235：150 合 1   */
/*                                                                   */
/*===================================================================*/

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 235  初始化映射器 235                          */
/*-------------------------------------------------------------------*/
void Map235_Init()
{
  /* Initialize Mapper  初始化映射器 */
  MapperInit = Map235_Init;

  /* Write to Mapper  写入映射器 */
  MapperWrite = Map235_Write;

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

  /* Set Registers  设置寄存器 */
  for( int i = 0; i < 0x2000; i++ ) {
    DRAM[i] = 0xFF;
  }

  /* Set ROM Banks  设置 ROM 银行 */
  ROMBANK0 = ROMPAGE( 0 );
  ROMBANK1 = ROMPAGE( 1 );
  ROMBANK2 = ROMPAGE( 2 );
  ROMBANK3 = ROMPAGE( 3 );

  /* Set up wiring of the interrupt pin  设置中断引脚接线方式 */
  K6502_Set_Int_Wiring( 1, 1 ); 
}

/*-------------------------------------------------------------------*/
/*  Mapper 235 Write Function  映射器 235：写函数                    */
/*-------------------------------------------------------------------*/
void Map235_Write( uint16_t address, uint8_t byte_data )
{
  uint8_t	prg = ((address&0x0300)>>3)|(address&0x001F);
  uint8_t	bus_state = 0;

  if( (NesHeader.rom_size<<1) == 64*2 ) {
    // 100-in-1
    switch( address & 0x0300 ) {
    case	0x0000:	break;
    case	0x0100:	bus_state = 1; break;
    case	0x0200:	bus_state = 1; break;
    case	0x0300:	bus_state = 1; break;
    }
  } else if( (NesHeader.rom_size<<1) == 128*2 ) {
    // 150-in-1
    switch( address & 0x0300 ) {
    case	0x0000:	break;
    case	0x0100:	bus_state = 1; break;
    case	0x0200:	prg = (prg&0x1F)|0x20; break;
    case	0x0300:	bus_state = 1; break;
    }
  } else if( (NesHeader.rom_size<<1) == 192*2 ) {
    // 150-in-1
    switch( address & 0x0300 ) {
    case	0x0000:	break;
    case	0x0100:	bus_state = 1; break;
    case	0x0200:	prg = (prg&0x1F)|0x20; break;
    case	0x0300:	prg = (prg&0x1F)|0x40; break;
    }
  } else if( (NesHeader.rom_size<<1) == 256*2 ) {
  }
  
  if( address & 0x0800 ) {
    if( address & 0x1000 ) {
      ROMBANK0 = ROMPAGE(((prg<<2)+2) % (NesHeader.rom_size<<1));
      ROMBANK1 = ROMPAGE(((prg<<2)+3) % (NesHeader.rom_size<<1));
      ROMBANK2 = ROMPAGE(((prg<<2)+2) % (NesHeader.rom_size<<1));
      ROMBANK3 = ROMPAGE(((prg<<2)+3) % (NesHeader.rom_size<<1));
    } else {
      ROMBANK0 = ROMPAGE(((prg<<2)+0) % (NesHeader.rom_size<<1));
      ROMBANK1 = ROMPAGE(((prg<<2)+1) % (NesHeader.rom_size<<1));
      ROMBANK2 = ROMPAGE(((prg<<2)+0) % (NesHeader.rom_size<<1));
      ROMBANK3 = ROMPAGE(((prg<<2)+1) % (NesHeader.rom_size<<1));
    }
  } else {
    ROMBANK0 = ROMPAGE(((prg<<2)+0) % (NesHeader.rom_size<<1));
    ROMBANK1 = ROMPAGE(((prg<<2)+1) % (NesHeader.rom_size<<1));
    ROMBANK2 = ROMPAGE(((prg<<2)+2) % (NesHeader.rom_size<<1));
    ROMBANK3 = ROMPAGE(((prg<<2)+3) % (NesHeader.rom_size<<1));
  }
  
  if( bus_state ) {
    ROMBANK0 = DRAM;
    ROMBANK1 = DRAM;
    ROMBANK2 = DRAM;
    ROMBANK3 = DRAM;
  }

  if( address & 0x0400 ) {
    InfoNES_Mirroring( 3 );
  } else if( address & 0x2000 ) {
    InfoNES_Mirroring( 0 );
  } else {
    InfoNES_Mirroring( 1 );
  }
}

