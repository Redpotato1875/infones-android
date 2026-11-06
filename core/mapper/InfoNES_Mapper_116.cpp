/*===================================================================*/
/*                                                                   */
/*   Mapper 116 : CartSaint : Yuu Yuu AV Kyoretsuden  映射器 116：CartSaint——幽游 AV 恐怖传 */
/*                                                                   */
/*===================================================================*/

uint8_t	Map116_Reg[8];
uint8_t	Map116_Prg0, Map116_Prg1, Map116_Prg2, Map116_Prg3;
uint8_t	Map116_Prg0L, Map116_Prg1L;
uint8_t	Map116_Chr0, Map116_Chr1, Map116_Chr2, Map116_Chr3;
uint8_t    Map116_Chr4, Map116_Chr5, Map116_Chr6, Map116_Chr7;

uint8_t	Map116_IRQ_Enable;
uint8_t	Map116_IRQ_Counter;
uint8_t	Map116_IRQ_Latch;

uint8_t	Map116_ExPrgSwitch;
uint8_t	Map116_ExChrSwitch;

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 116  初始化映射器 116                          */
/*-------------------------------------------------------------------*/
void Map116_Init()
{
  /* Initialize Mapper  初始化映射器 */
  MapperInit = Map116_Init;

  /* Write to Mapper  写入映射器 */
  MapperWrite = Map116_Write;

  /* Write to SRAM  写入 SRAM */
  MapperSram = Map0_Sram;

  /* Write to APU  写入 APU */
  MapperApu = Map0_Apu;

  /* Read from APU  从 APU 读取 */
  MapperReadApu = Map0_ReadApu;

  /* Callback at VSync  场同步回调 */
  MapperVSync = Map0_VSync;

  /* Callback at HSync  行同步回调 */
  MapperHSync = Map116_HSync;

  /* Callback at PPU  PPU 回调 */
  MapperPPU = Map0_PPU;

  /* Callback at Rendering Screen ( 1:BG, 0:Sprite )  渲染画面回调（1：背景，0：精灵） */
  MapperRenderScreen = Map0_RenderScreen;

  /* Set SRAM Banks  设置 SRAM 银行 */
  SRAMBANK = SRAM;

  /* Initialize Registers  初始化寄存器 */
  for( int i = 0; i < 8; i++ ) {
    Map116_Reg[i] = 0x00;
  }

  Map116_Prg0 = Map116_Prg0L = 0;
  Map116_Prg1 = Map116_Prg1L = 1;
  Map116_Prg2 = ( NesHeader.rom_size << 1 ) - 2;
  Map116_Prg3 = ( NesHeader.rom_size << 1 ) - 1;

  Map116_ExPrgSwitch = 0;
  Map116_ExChrSwitch = 0;

  /* Set ROM Banks  设置 ROM 银行 */
  Map116_Set_CPU_Banks();

  /* Set PPU Banks  设置 PPU 银行 */
  if ( NesHeader.v_rom_size > 0 ) {
    Map116_Chr0 = 0;
    Map116_Chr1 = 1;
    Map116_Chr2 = 2;
    Map116_Chr3 = 3;
    Map116_Chr4 = 4;
    Map116_Chr5 = 5;
    Map116_Chr6 = 6;
    Map116_Chr7 = 7;
    Map116_Set_PPU_Banks();
  } else {
    Map116_Chr0 = Map116_Chr2 = Map116_Chr4 = Map116_Chr5 = Map116_Chr6 = Map116_Chr7 = 0;
    Map116_Chr1 = Map116_Chr3 = 1;
  }

  Map116_IRQ_Enable = 0;	/* Disable  禁止 */
  Map116_IRQ_Counter = 0;
  Map116_IRQ_Latch = 0;
  
  /* Set up wiring of the interrupt pin  设置中断引脚接线方式 */
  K6502_Set_Int_Wiring( 1, 1 ); 
}

/*-------------------------------------------------------------------*/
/*  Mapper 116 Write Function  映射器 116：写函数                    */
/*-------------------------------------------------------------------*/
void Map116_Write( uint16_t address, uint8_t byte_data )
{
  switch( address & 0xE001 ) {
  case	0x8000:
    Map116_Reg[0] = byte_data;
    Map116_Set_CPU_Banks();
    Map116_Set_PPU_Banks();
    break;
  case	0x8001:
    Map116_Reg[1] = byte_data;
    switch( Map116_Reg[0] & 0x07 ) {
    case	0x00:
      Map116_Chr0 = byte_data & 0xFE;
      Map116_Chr1 = Map116_Chr0+1;
      Map116_Set_PPU_Banks();
      break;
    case	0x01:
      Map116_Chr2 = byte_data & 0xFE;
      Map116_Chr3 = Map116_Chr2+1;
      Map116_Set_PPU_Banks();
      break;
    case	0x02:
      Map116_Chr4 = byte_data;
      Map116_Set_PPU_Banks();
      break;
    case	0x03:
      Map116_Chr5 = byte_data;
      Map116_Set_PPU_Banks();
      break;
    case	0x04:
      Map116_Chr6 = byte_data;
      Map116_Set_PPU_Banks();
      break;
    case	0x05:
      Map116_Chr7 = byte_data;
      Map116_Set_PPU_Banks();
      break;
    case	0x06:
      Map116_Prg0 = byte_data;
      Map116_Set_CPU_Banks();
      break;
    case	0x07:
      Map116_Prg1 = byte_data;
      Map116_Set_CPU_Banks();
      break;
    }
    break;
  case	0xA000:
    Map116_Reg[2] = byte_data;
    if ( !ROM_FourScr ) {
      if( byte_data & 0x01 ) InfoNES_Mirroring( 0 );
      else		  InfoNES_Mirroring( 1 );
    }
    break;
  case	0xA001:
    Map116_Reg[3] = byte_data;
    break;
  case	0xC000:
    Map116_Reg[4] = byte_data;
    Map116_IRQ_Counter = byte_data;
    Map116_IRQ_Enable = 0xFF;
    break;
  case	0xC001:
    Map116_Reg[5] = byte_data;
    Map116_IRQ_Latch = byte_data;
    break;
  case	0xE000:
    Map116_Reg[6] = byte_data;
    Map116_IRQ_Enable = 0;
    break;
  case	0xE001:
    Map116_Reg[7] = byte_data;
    Map116_IRQ_Enable = 0xFF;
    break;
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 116 H-Sync Function  映射器 116：行同步函数               */
/*-------------------------------------------------------------------*/
void Map116_HSync()
{
  if( (PPU_Scanline >= 0 && PPU_Scanline <= 239) ) {
    if( PPU_R1 & R1_SHOW_SCR || PPU_R1 & R1_SHOW_SP ) {
      if( Map116_IRQ_Enable ) {
	if( !(Map116_IRQ_Counter--) ) {
	  Map116_IRQ_Counter = Map116_IRQ_Latch;
	  IRQ_REQ;
	}
      }
    }
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 116 Set CPU Banks Function  映射器 116：设置 CPU 银行函数 */
/*-------------------------------------------------------------------*/
void Map116_Set_CPU_Banks()
{
  if( Map116_Reg[0] & 0x40 ) {
    ROMBANK0 = ROMPAGE( ( NesHeader.rom_size << 1 ) - 2 );
    ROMBANK1 = ROMPAGE( Map116_Prg1 % ( NesHeader.rom_size << 1 ) );
    ROMBANK2 = ROMPAGE( Map116_Prg0 % ( NesHeader.rom_size << 1 ) );
    ROMBANK3 = ROMPAGE( ( NesHeader.rom_size << 1 ) - 1 );
  } else {
    ROMBANK0 = ROMPAGE( Map116_Prg0 % ( NesHeader.rom_size << 1 ) );
    ROMBANK1 = ROMPAGE( Map116_Prg1 % ( NesHeader.rom_size << 1 ) );
    ROMBANK2 = ROMPAGE( ( NesHeader.rom_size << 1 ) - 2 );
    ROMBANK3 = ROMPAGE( ( NesHeader.rom_size << 1 ) - 1 );
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 116 Set PPU Banks Function  映射器 116：设置 PPU 银行函数 */
/*-------------------------------------------------------------------*/
void Map116_Set_PPU_Banks()
{
  if ( NesHeader.v_rom_size > 0 ) {
    if( Map116_Reg[0] & 0x80 ) {
      PPUBANK[ 0 ] = VROMPAGE( ((Map116_ExChrSwitch<<8)+Map116_Chr4) % (NesHeader.v_rom_size<<3));
      PPUBANK[ 1 ] = VROMPAGE( ((Map116_ExChrSwitch<<8)+Map116_Chr5) % (NesHeader.v_rom_size<<3));
      PPUBANK[ 2 ] = VROMPAGE( ((Map116_ExChrSwitch<<8)+Map116_Chr6) % (NesHeader.v_rom_size<<3));
      PPUBANK[ 3 ] = VROMPAGE( ((Map116_ExChrSwitch<<8)+Map116_Chr7) % (NesHeader.v_rom_size<<3));
      PPUBANK[ 4 ] = VROMPAGE( ((Map116_ExChrSwitch<<8)+Map116_Chr0) % (NesHeader.v_rom_size<<3));
      PPUBANK[ 5 ] = VROMPAGE( ((Map116_ExChrSwitch<<8)+Map116_Chr1) % (NesHeader.v_rom_size<<3));
      PPUBANK[ 6 ] = VROMPAGE( ((Map116_ExChrSwitch<<8)+Map116_Chr2) % (NesHeader.v_rom_size<<3));
      PPUBANK[ 7 ] = VROMPAGE( ((Map116_ExChrSwitch<<8)+Map116_Chr3) % (NesHeader.v_rom_size<<3));
      InfoNES_SetupChr();
    } else {
      PPUBANK[ 0 ] = VROMPAGE( ((Map116_ExChrSwitch<<8)+Map116_Chr0) % (NesHeader.v_rom_size<<3));
      PPUBANK[ 1 ] = VROMPAGE( ((Map116_ExChrSwitch<<8)+Map116_Chr1) % (NesHeader.v_rom_size<<3));
      PPUBANK[ 2 ] = VROMPAGE( ((Map116_ExChrSwitch<<8)+Map116_Chr2) % (NesHeader.v_rom_size<<3));
      PPUBANK[ 3 ] = VROMPAGE( ((Map116_ExChrSwitch<<8)+Map116_Chr3) % (NesHeader.v_rom_size<<3));
      PPUBANK[ 4 ] = VROMPAGE( ((Map116_ExChrSwitch<<8)+Map116_Chr4) % (NesHeader.v_rom_size<<3));
      PPUBANK[ 5 ] = VROMPAGE( ((Map116_ExChrSwitch<<8)+Map116_Chr5) % (NesHeader.v_rom_size<<3));
      PPUBANK[ 6 ] = VROMPAGE( ((Map116_ExChrSwitch<<8)+Map116_Chr6) % (NesHeader.v_rom_size<<3));
      PPUBANK[ 7 ] = VROMPAGE( ((Map116_ExChrSwitch<<8)+Map116_Chr7) % (NesHeader.v_rom_size<<3));
      InfoNES_SetupChr();
    }
  }
}

