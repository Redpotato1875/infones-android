/*===================================================================*/
/*                                                                   */
/*                     Mapper 119 : TQ-ROM  映射器 119：TQ-ROM       */
/*                                                                   */
/*===================================================================*/

uint8_t	Map119_Reg[8];
uint8_t	Map119_Prg0, Map119_Prg1;
uint8_t	Map119_Chr01, Map119_Chr23, Map119_Chr4, Map119_Chr5, Map119_Chr6, Map119_Chr7;
uint8_t	Map119_WeSram;

uint8_t	Map119_IRQ_Enable;
uint8_t	Map119_IRQ_Counter;
uint8_t	Map119_IRQ_Latch;

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 119  初始化映射器 119                          */
/*-------------------------------------------------------------------*/
void Map119_Init()
{
  /* Initialize Mapper  初始化映射器 */
  MapperInit = Map119_Init;

  /* Write to Mapper  写入映射器 */
  MapperWrite = Map119_Write;

  /* Write to SRAM  写入 SRAM */
  MapperSram = Map0_Sram;

  /* Write to APU  写入 APU */
  MapperApu = Map0_Apu;

  /* Read from APU  从 APU 读取 */
  MapperReadApu = Map0_ReadApu;

  /* Callback at VSync  场同步回调 */
  MapperVSync = Map0_VSync;

  /* Callback at HSync  行同步回调 */
  MapperHSync = Map119_HSync;

  /* Callback at PPU  PPU 回调 */
  MapperPPU = Map0_PPU;

  /* Callback at Rendering Screen ( 1:BG, 0:Sprite )  渲染画面回调（1：背景，0：精灵） */
  MapperRenderScreen = Map0_RenderScreen;

  /* Set SRAM Banks  设置 SRAM 银行 */
  SRAMBANK = SRAM;

  /* Set Registers  设置寄存器 */
  for( int i = 0; i < 8; i++ ) {
    Map119_Reg[i] = 0x00;
  }

  /* Set ROM Banks  设置 ROM 银行 */
  Map119_Prg0 = 0;
  Map119_Prg1 = 1;
  Map119_Set_CPU_Banks();

  /* Set PPU Banks  设置 PPU 银行 */
  Map119_Chr01 = 0;
  Map119_Chr23 = 2;
  Map119_Chr4  = 4;
  Map119_Chr5  = 5;
  Map119_Chr6  = 6;
  Map119_Chr7  = 7;
  Map119_Set_PPU_Banks();

  Map119_WeSram  = 0;		// Disable  禁止
  Map119_IRQ_Enable = 0;	// Disable  禁止
  Map119_IRQ_Counter = 0;
  Map119_IRQ_Latch = 0;

  /* Set up wiring of the interrupt pin  设置中断引脚接线方式 */
  K6502_Set_Int_Wiring( 1, 1 ); 
}

/*-------------------------------------------------------------------*/
/*  Mapper 119 Write Function  映射器 119：写函数                    */
/*-------------------------------------------------------------------*/
void Map119_Write( uint16_t address, uint8_t byte_data )
{
  switch( address & 0xE001 ) {
  case	0x8000:
    Map119_Reg[0] = byte_data;
    Map119_Set_CPU_Banks();
    Map119_Set_PPU_Banks();
    break;
  case	0x8001:
    Map119_Reg[1] = byte_data;
    
    switch( Map119_Reg[0] & 0x07 ) {
    case	0x00:
      if( NesHeader.v_rom_size > 0 ) {
	Map119_Chr01 = byte_data & 0xFE;
	Map119_Set_PPU_Banks();
      }
      break;
    case	0x01:
      if( NesHeader.v_rom_size > 0 ) {
	Map119_Chr23 = byte_data & 0xFE;
	Map119_Set_PPU_Banks();
      }
      break;
    case	0x02:
      if( NesHeader.v_rom_size > 0 ) {
	Map119_Chr4 = byte_data;
	Map119_Set_PPU_Banks();
      }
      break;
    case	0x03:
      if( NesHeader.v_rom_size > 0 ) {
	Map119_Chr5 = byte_data;
	Map119_Set_PPU_Banks();
      }
      break;
    case	0x04:
      if( NesHeader.v_rom_size > 0 ) {
	Map119_Chr6 = byte_data;
	Map119_Set_PPU_Banks();
      }
      break;
    case	0x05:
      if( NesHeader.v_rom_size > 0 ) {
	Map119_Chr7 = byte_data;
	Map119_Set_PPU_Banks();
      }
      break;
    case	0x06:
      Map119_Prg0 = byte_data;
      Map119_Set_CPU_Banks();
      break;
    case	0x07:
      Map119_Prg1 = byte_data;
      Map119_Set_CPU_Banks();
      break;
    }
    break;
  case	0xA000:
    Map119_Reg[2] = byte_data;
    if( !ROM_FourScr ) {
      if( byte_data & 0x01 ) InfoNES_Mirroring( 0 );
      else		  InfoNES_Mirroring( 1 );
    }
    break;
  case	0xA001:
    Map119_Reg[3] = byte_data;
    break;
  case	0xC000:
    Map119_Reg[4] = byte_data;
    Map119_IRQ_Counter = byte_data;
    break;
  case	0xC001:
    Map119_Reg[5] = byte_data;
    Map119_IRQ_Latch = byte_data;
    break;
  case	0xE000:
    Map119_Reg[6] = byte_data;
    Map119_IRQ_Enable = 0;
    Map119_IRQ_Counter = Map119_IRQ_Latch;
    break;
  case	0xE001:
    Map119_Reg[7] = byte_data;
    Map119_IRQ_Enable = 1;
    break;
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 119 H-Sync Function  映射器 119：行同步函数               */
/*-------------------------------------------------------------------*/
void Map119_HSync()
{
  if( (PPU_Scanline >= 0 && PPU_Scanline <= 239) ) {
    if( PPU_R1 & R1_SHOW_SCR || PPU_R1 & R1_SHOW_SP ) {
      if( Map119_IRQ_Enable ) {
	if( !(Map119_IRQ_Counter--) ) {
	  Map119_IRQ_Counter = Map119_IRQ_Latch;
	  IRQ_REQ;
	}
      }
    }
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 119 Set CPU Banks Function  映射器 119：设置 CPU 银行函数 */
/*-------------------------------------------------------------------*/
void Map119_Set_CPU_Banks()
{
  if( Map119_Reg[0] & 0x40 ) {
    ROMBANK0 = ROMLASTPAGE( 1 );
    ROMBANK1 = ROMPAGE( Map119_Prg1 % ( NesHeader.rom_size << 1 ) );
    ROMBANK2 = ROMPAGE( Map119_Prg0 % ( NesHeader.rom_size << 1 ) );
    ROMBANK3 = ROMLASTPAGE( 0 );
  } else {
    ROMBANK0 = ROMPAGE( Map119_Prg0 % ( NesHeader.rom_size << 1 ) );
    ROMBANK1 = ROMPAGE( Map119_Prg1 % ( NesHeader.rom_size << 1 ) );
    ROMBANK2 = ROMLASTPAGE( 1 );
    ROMBANK3 = ROMLASTPAGE( 0 );
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 119 Set PPU Banks Function  映射器 119：设置 PPU 银行函数 */
/*-------------------------------------------------------------------*/
void Map119_Set_PPU_Banks()
{
  if( Map119_Reg[0]&0x80 ) {
    if(Map119_Chr4&0x40)	PPUBANK[ 0 ] = CRAMPAGE(Map119_Chr4&0x07);
    else			PPUBANK[ 0 ] = VROMPAGE(Map119_Chr4 % (NesHeader.rom_size<<1));
    if(Map119_Chr5&0x40)	PPUBANK[ 1 ] = CRAMPAGE(Map119_Chr5&0x07);
    else			PPUBANK[ 1 ] = VROMPAGE(Map119_Chr5 % (NesHeader.rom_size<<1));
    if(Map119_Chr6&0x40)	PPUBANK[ 2 ] = CRAMPAGE(Map119_Chr6&0x07);
    else			PPUBANK[ 2 ] = VROMPAGE(Map119_Chr6 % (NesHeader.rom_size<<1));
    if(Map119_Chr7&0x40)	PPUBANK[ 3 ] = CRAMPAGE(Map119_Chr7&0x07);
    else			PPUBANK[ 3 ] = VROMPAGE(Map119_Chr7 % (NesHeader.rom_size<<1));
    
    if((Map119_Chr01+0)&0x40)   PPUBANK[ 4 ] = CRAMPAGE((Map119_Chr01+0)&0x07);
    else		        PPUBANK[ 4 ] = VROMPAGE((Map119_Chr01+0) % (NesHeader.v_rom_size<<3));
    if((Map119_Chr01+1)&0x40)   PPUBANK[ 5 ] = CRAMPAGE((Map119_Chr01+1)&0x07);
    else		     	PPUBANK[ 5 ] = VROMPAGE((Map119_Chr01+1) % (NesHeader.v_rom_size<<3));
    if((Map119_Chr23+0)&0x40)   PPUBANK[ 6 ] = CRAMPAGE((Map119_Chr23+0)&0x07);
    else			PPUBANK[ 6 ] = VROMPAGE((Map119_Chr23+0) % (NesHeader.v_rom_size<<3));
    if((Map119_Chr23+1)&0x40)   PPUBANK[ 7 ] = CRAMPAGE((Map119_Chr23+1)&0x07);
    else			PPUBANK[ 7 ] = VROMPAGE((Map119_Chr23+1) % (NesHeader.v_rom_size<<3));
  } else {
    if((Map119_Chr01+0)&0x40)   PPUBANK[ 0 ] = CRAMPAGE((Map119_Chr01+0)&0x07);
    else			PPUBANK[ 0 ] = VROMPAGE((Map119_Chr01+0) % (NesHeader.v_rom_size<<3));
    if((Map119_Chr01+1)&0x40)   PPUBANK[ 1 ] = CRAMPAGE((Map119_Chr01+1)&0x07);
    else			PPUBANK[ 1 ] = VROMPAGE((Map119_Chr01+1) % (NesHeader.v_rom_size<<3));
    if((Map119_Chr23+0)&0x40)   PPUBANK[ 2 ] = CRAMPAGE((Map119_Chr23+0)&0x07);
    else		     	PPUBANK[ 2 ] = VROMPAGE((Map119_Chr23+0) % (NesHeader.v_rom_size<<3));
    if((Map119_Chr23+1)&0x40)   PPUBANK[ 3 ] = CRAMPAGE((Map119_Chr23+1)&0x07);
    else		     	PPUBANK[ 3 ] = VROMPAGE((Map119_Chr23+1) % (NesHeader.v_rom_size<<3));
    
    if(Map119_Chr4&0x40)	PPUBANK[ 4 ] = CRAMPAGE(Map119_Chr4&0x07);
    else			PPUBANK[ 4 ] = VROMPAGE(Map119_Chr4 % (NesHeader.v_rom_size<<3));
    if(Map119_Chr5&0x40)	PPUBANK[ 5 ] = CRAMPAGE(Map119_Chr5&0x07);
    else			PPUBANK[ 5 ] = VROMPAGE(Map119_Chr5 % (NesHeader.v_rom_size<<3));
    if(Map119_Chr6&0x40)	PPUBANK[ 6 ] = CRAMPAGE(Map119_Chr6&0x07);
    else			PPUBANK[ 6 ] = VROMPAGE(Map119_Chr6 % (NesHeader.v_rom_size<<3));
    if(Map119_Chr7&0x40)	PPUBANK[ 7 ] = CRAMPAGE(Map119_Chr7&0x07);
    else			PPUBANK[ 7 ] = VROMPAGE(Map119_Chr7 % (NesHeader.v_rom_size<<3));
  }
  InfoNES_SetupChr();
}

