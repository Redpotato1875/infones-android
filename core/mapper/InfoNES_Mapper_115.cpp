/*===================================================================*/
/*                                                                   */
/*   Mapper 115 : CartSaint : Yuu Yuu Hakusho Final JusticePao(?)  映射器 115：CartSaint——幽游白书 最终正义（？） */
/*                                                                   */
/*===================================================================*/

uint8_t	Map115_Reg[8];
uint8_t	Map115_Prg0, Map115_Prg1, Map115_Prg2, Map115_Prg3;
uint8_t	Map115_Prg0L, Map115_Prg1L;
uint8_t	Map115_Chr0, Map115_Chr1, Map115_Chr2, Map115_Chr3;
uint8_t    Map115_Chr4, Map115_Chr5, Map115_Chr6, Map115_Chr7;

uint8_t	Map115_IRQ_Enable;
uint8_t	Map115_IRQ_Counter;
uint8_t	Map115_IRQ_Latch;

uint8_t	Map115_ExPrgSwitch;
uint8_t	Map115_ExChrSwitch;

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 115  初始化映射器 115                          */
/*-------------------------------------------------------------------*/
void Map115_Init()
{
  /* Initialize Mapper  初始化映射器 */
  MapperInit = Map115_Init;

  /* Write to Mapper  写入映射器 */
  MapperWrite = Map115_Write;

  /* Write to SRAM  写入 SRAM */
  MapperSram = Map115_Sram;

  /* Write to APU  写入 APU */
  MapperApu = Map0_Apu;

  /* Read from APU  从 APU 读取 */
  MapperReadApu = Map0_ReadApu;

  /* Callback at VSync  场同步回调 */
  MapperVSync = Map0_VSync;

  /* Callback at HSync  行同步回调 */
  MapperHSync = Map115_HSync;

  /* Callback at PPU  PPU 回调 */
  MapperPPU = Map0_PPU;

  /* Callback at Rendering Screen ( 1:BG, 0:Sprite )  渲染画面回调（1：背景，0：精灵） */
  MapperRenderScreen = Map0_RenderScreen;

  /* Set SRAM Banks  设置 SRAM 银行 */
  SRAMBANK = SRAM;

  /* Initialize Registers  初始化寄存器 */
  for( int i = 0; i < 8; i++ ) {
    Map115_Reg[i] = 0x00;
  }

  Map115_Prg0 = Map115_Prg0L = 0;
  Map115_Prg1 = Map115_Prg1L = 1;
  Map115_Prg2 = ( NesHeader.rom_size << 1 ) - 2;
  Map115_Prg3 = ( NesHeader.rom_size << 1 ) - 1;

  Map115_ExPrgSwitch = 0;
  Map115_ExChrSwitch = 0;

  /* Set ROM Banks  设置 ROM 银行 */
  Map115_Set_CPU_Banks();

  /* Set PPU Banks  设置 PPU 银行 */
  if ( NesHeader.v_rom_size > 0 ) {
    Map115_Chr0 = 0;
    Map115_Chr1 = 1;
    Map115_Chr2 = 2;
    Map115_Chr3 = 3;
    Map115_Chr4 = 4;
    Map115_Chr5 = 5;
    Map115_Chr6 = 6;
    Map115_Chr7 = 7;
    Map115_Set_PPU_Banks();
  } else {
    Map115_Chr0 = Map115_Chr2 = Map115_Chr4 = Map115_Chr5 = Map115_Chr6 = Map115_Chr7 = 0;
    Map115_Chr1 = Map115_Chr3 = 1;
  }

  Map115_IRQ_Enable = 0;	/* Disable  禁止 */
  Map115_IRQ_Counter = 0;
  Map115_IRQ_Latch = 0;
  
  /* Set up wiring of the interrupt pin  设置中断引脚接线方式 */
  K6502_Set_Int_Wiring( 1, 1 ); 
}

/*-------------------------------------------------------------------*/
/*  Mapper 115 Write Function  映射器 115：写函数                    */
/*-------------------------------------------------------------------*/
void Map115_Write( uint16_t address, uint8_t byte_data )
{
  switch( address & 0xE001 ) {
  case	0x8000:
    Map115_Reg[0] = byte_data;
    Map115_Set_CPU_Banks();
    Map115_Set_PPU_Banks();
    break;
  case	0x8001:
    Map115_Reg[1] = byte_data;
    switch( Map115_Reg[0] & 0x07 ) {
    case	0x00:
      Map115_Chr0 = byte_data & 0xFE;
      Map115_Chr1 = Map115_Chr0+1;
      Map115_Set_PPU_Banks();
      break;
    case	0x01:
      Map115_Chr2 = byte_data & 0xFE;
      Map115_Chr3 = Map115_Chr2+1;
      Map115_Set_PPU_Banks();
      break;
    case	0x02:
      Map115_Chr4 = byte_data;
      Map115_Set_PPU_Banks();
      break;
    case	0x03:
      Map115_Chr5 = byte_data;
      Map115_Set_PPU_Banks();
      break;
    case	0x04:
      Map115_Chr6 = byte_data;
      Map115_Set_PPU_Banks();
      break;
    case	0x05:
      Map115_Chr7 = byte_data;
      Map115_Set_PPU_Banks();
      break;
    case	0x06:
      Map115_Prg0 = Map115_Prg0L = byte_data;
      Map115_Set_CPU_Banks();
      break;
    case	0x07:
      Map115_Prg1 = Map115_Prg1L = byte_data;
      Map115_Set_CPU_Banks();
      break;
    }
    break;
  case	0xA000:
    Map115_Reg[2] = byte_data;
    if ( !ROM_FourScr ) {
      if( byte_data & 0x01 ) InfoNES_Mirroring( 0 );
      else		  InfoNES_Mirroring( 1 );
    }
    break;
  case	0xA001:
    Map115_Reg[3] = byte_data;
    break;
  case	0xC000:
    Map115_Reg[4] = byte_data;
    Map115_IRQ_Counter = byte_data;
    Map115_IRQ_Enable = 0xFF;
    break;
  case	0xC001:
    Map115_Reg[5] = byte_data;
    Map115_IRQ_Latch = byte_data;
    break;
  case	0xE000:
    Map115_Reg[6] = byte_data;
    Map115_IRQ_Enable = 0;
    break;
  case	0xE001:
    Map115_Reg[7] = byte_data;
    Map115_IRQ_Enable = 0xFF;
    break;
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 115 Write to SRAM Function  映射器 115：写 SRAM 函数      */
/*-------------------------------------------------------------------*/
void Map115_Sram( uint16_t address, uint8_t byte_data )
{
  switch ( address ) {
  case	0x6000:
    Map115_ExPrgSwitch = byte_data;
    Map115_Set_CPU_Banks();
    break;
  case	0x6001:
    Map115_ExChrSwitch = byte_data&0x1;
    Map115_Set_PPU_Banks();
    break;
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 115 H-Sync Function  映射器 115：行同步函数               */
/*-------------------------------------------------------------------*/
void Map115_HSync()
{
  if( (PPU_Scanline >= 0 && PPU_Scanline <= 239) ) {
    if( PPU_R1 & R1_SHOW_SCR || PPU_R1 & R1_SHOW_SP ) {
      if( Map115_IRQ_Enable ) {
	if( !(Map115_IRQ_Counter--) ) {
	  Map115_IRQ_Counter = Map115_IRQ_Latch;
	  IRQ_REQ;
	}
      }
    }
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 115 Set CPU Banks Function  映射器 115：设置 CPU 银行函数 */
/*-------------------------------------------------------------------*/
void Map115_Set_CPU_Banks()
{
  if( Map115_ExPrgSwitch & 0x80 ) {
    Map115_Prg0 = ((Map115_ExPrgSwitch<<1)&0x1e);
    Map115_Prg1 = Map115_Prg0+1;

    ROMBANK0 = ROMPAGE( Map115_Prg0 % ( NesHeader.rom_size << 1 ) );
    ROMBANK1 = ROMPAGE( Map115_Prg1 % ( NesHeader.rom_size << 1 ) );
    ROMBANK2 = ROMPAGE( ( Map115_Prg0+2 ) % ( NesHeader.rom_size << 1 ) );
    ROMBANK3 = ROMPAGE( ( Map115_Prg1+2 ) % ( NesHeader.rom_size << 1 ) );
  } else {
    Map115_Prg0 = Map115_Prg0L;
    Map115_Prg1 = Map115_Prg1L;
    if( Map115_Reg[0] & 0x40 ) {
      ROMBANK0 = ROMPAGE( ( NesHeader.rom_size << 1 ) - 2 );
      ROMBANK1 = ROMPAGE( Map115_Prg1 % ( NesHeader.rom_size << 1 ) );
      ROMBANK2 = ROMPAGE( Map115_Prg0 % ( NesHeader.rom_size << 1 ) );
      ROMBANK3 = ROMPAGE( ( NesHeader.rom_size << 1 ) - 1 );
    } else {
      ROMBANK0 = ROMPAGE( Map115_Prg0 % ( NesHeader.rom_size << 1 ) );
      ROMBANK1 = ROMPAGE( Map115_Prg1 % ( NesHeader.rom_size << 1 ) );
      ROMBANK2 = ROMPAGE( ( NesHeader.rom_size << 1 ) - 2 );
      ROMBANK3 = ROMPAGE( ( NesHeader.rom_size << 1 ) - 1 );
    }
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 115 Set PPU Banks Function  映射器 115：设置 PPU 银行函数 */
/*-------------------------------------------------------------------*/
void Map115_Set_PPU_Banks()
{
  if ( NesHeader.v_rom_size > 0 ) {
    if( Map115_Reg[0] & 0x80 ) {
      PPUBANK[ 0 ] = VROMPAGE( ((Map115_ExChrSwitch<<8)+Map115_Chr4) % (NesHeader.v_rom_size<<3));
      PPUBANK[ 1 ] = VROMPAGE( ((Map115_ExChrSwitch<<8)+Map115_Chr5) % (NesHeader.v_rom_size<<3));
      PPUBANK[ 2 ] = VROMPAGE( ((Map115_ExChrSwitch<<8)+Map115_Chr6) % (NesHeader.v_rom_size<<3));
      PPUBANK[ 3 ] = VROMPAGE( ((Map115_ExChrSwitch<<8)+Map115_Chr7) % (NesHeader.v_rom_size<<3));
      PPUBANK[ 4 ] = VROMPAGE( ((Map115_ExChrSwitch<<8)+Map115_Chr0) % (NesHeader.v_rom_size<<3));
      PPUBANK[ 5 ] = VROMPAGE( ((Map115_ExChrSwitch<<8)+Map115_Chr1) % (NesHeader.v_rom_size<<3));
      PPUBANK[ 6 ] = VROMPAGE( ((Map115_ExChrSwitch<<8)+Map115_Chr2) % (NesHeader.v_rom_size<<3));
      PPUBANK[ 7 ] = VROMPAGE( ((Map115_ExChrSwitch<<8)+Map115_Chr3) % (NesHeader.v_rom_size<<3));
      InfoNES_SetupChr();
    } else {
      PPUBANK[ 0 ] = VROMPAGE( ((Map115_ExChrSwitch<<8)+Map115_Chr0) % (NesHeader.v_rom_size<<3));
      PPUBANK[ 1 ] = VROMPAGE( ((Map115_ExChrSwitch<<8)+Map115_Chr1) % (NesHeader.v_rom_size<<3));
      PPUBANK[ 2 ] = VROMPAGE( ((Map115_ExChrSwitch<<8)+Map115_Chr2) % (NesHeader.v_rom_size<<3));
      PPUBANK[ 3 ] = VROMPAGE( ((Map115_ExChrSwitch<<8)+Map115_Chr3) % (NesHeader.v_rom_size<<3));
      PPUBANK[ 4 ] = VROMPAGE( ((Map115_ExChrSwitch<<8)+Map115_Chr4) % (NesHeader.v_rom_size<<3));
      PPUBANK[ 5 ] = VROMPAGE( ((Map115_ExChrSwitch<<8)+Map115_Chr5) % (NesHeader.v_rom_size<<3));
      PPUBANK[ 6 ] = VROMPAGE( ((Map115_ExChrSwitch<<8)+Map115_Chr6) % (NesHeader.v_rom_size<<3));
      PPUBANK[ 7 ] = VROMPAGE( ((Map115_ExChrSwitch<<8)+Map115_Chr7) % (NesHeader.v_rom_size<<3));
      InfoNES_SetupChr();
    }
  }
}

