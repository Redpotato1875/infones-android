/*===================================================================*/
/*                                                                   */
/*            Mapper 187 : Street Fighter Zero 2 97  映射器 187：街头霸王 Zero 2 97 */
/*                                                                   */
/*===================================================================*/

uint8_t	Map187_Prg[4];
int	Map187_Chr[8];
uint8_t	Map187_Bank[8];

uint8_t	Map187_ExtMode;
uint8_t	Map187_ChrMode;
uint8_t	Map187_ExtEnable;

uint8_t	Map187_IRQ_Enable;
uint8_t	Map187_IRQ_Counter;
uint8_t	Map187_IRQ_Latch;
uint8_t	Map187_IRQ_Occur;
uint8_t	Map187_LastWrite;

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 187  初始化映射器 187                          */
/*-------------------------------------------------------------------*/
void Map187_Init()
{
  /* Initialize Mapper  初始化映射器 */
  MapperInit = Map187_Init;

  /* Write to Mapper  写入映射器 */
  MapperWrite = Map187_Write;

  /* Write to SRAM  写入 SRAM */
  MapperSram = Map0_Sram;

  /* Write to APU  写入 APU */
  MapperApu = Map187_Apu;

  /* Read from APU  从 APU 读取 */
  MapperReadApu = Map187_ReadApu;

  /* Callback at VSync  场同步回调 */
  MapperVSync = Map0_VSync;

  /* Callback at HSync  行同步回调 */
  MapperHSync = Map187_HSync;

  /* Callback at PPU  PPU 回调 */
  MapperPPU = Map0_PPU;

  /* Callback at Rendering Screen ( 1:BG, 0:Sprite )  渲染画面回调（1：背景，0：精灵） */
  MapperRenderScreen = Map0_RenderScreen;

  /* Set SRAM Banks  设置 SRAM 银行 */
  SRAMBANK = SRAM;

  /* Set Registers  设置寄存器 */
  for( int i = 0; i < 8; i++ ) {
    Map187_Chr[i] = 0x00;
    Map187_Bank[i] = 0x00;
  }

  /* Set ROM Banks  设置 ROM 银行 */
  Map187_Prg[0] = (NesHeader.rom_size<<1)-4;
  Map187_Prg[1] = (NesHeader.rom_size<<1)-3;
  Map187_Prg[2] = (NesHeader.rom_size<<1)-2;
  Map187_Prg[3] = (NesHeader.rom_size<<1)-1;
  Map187_Set_CPU_Banks();

  Map187_ExtMode = 0;
  Map187_ChrMode = 0;
  Map187_ExtEnable = 0;

  Map187_IRQ_Enable = 0;
  Map187_IRQ_Counter = 0;
  Map187_IRQ_Latch = 0;

  Map187_LastWrite = 0;

  /* Set up wiring of the interrupt pin  设置中断引脚接线方式 */
  K6502_Set_Int_Wiring( 1, 1 ); 
}

/*-------------------------------------------------------------------*/
/*  Mapper 187 Write Function  映射器 187：写函数                    */
/*-------------------------------------------------------------------*/
void Map187_Write( uint16_t address, uint8_t byte_data )
{
  Map187_LastWrite = byte_data;
  switch( address ) {
  case	0x8003:
    Map187_ExtEnable = 0xFF;
    Map187_ChrMode = byte_data;
    if( (byte_data&0xF0) == 0 ) {
      Map187_Prg[2] = (NesHeader.rom_size<<1)-2;
      Map187_Set_CPU_Banks();
    }
    break;
    
  case	0x8000:
    Map187_ExtEnable = 0;
    Map187_ChrMode = byte_data;
    break;

  case	0x8001:
    if( !Map187_ExtEnable ) {
      switch( Map187_ChrMode & 7 ) {
      case	0:
	byte_data &= 0xFE;
	Map187_Chr[4] = (int)byte_data+0x100;
	Map187_Chr[5] = (int)byte_data+0x100+1;
	Map187_Set_PPU_Banks();
	break;
      case	1:
	byte_data &= 0xFE;
	Map187_Chr[6] = (int)byte_data+0x100;
	Map187_Chr[7] = (int)byte_data+0x100+1;
	Map187_Set_PPU_Banks();
	break;
      case	2:
	Map187_Chr[0] = byte_data;
	Map187_Set_PPU_Banks();
	break;
      case	3:
	Map187_Chr[1] = byte_data;
	Map187_Set_PPU_Banks();
	break;
      case	4:
	Map187_Chr[2] = byte_data;
	Map187_Set_PPU_Banks();
	break;
      case	5:
	Map187_Chr[3] = byte_data;
	Map187_Set_PPU_Banks();
	break;
      case	6:
	if( (Map187_ExtMode&0xA0)!=0xA0 ) {
	  Map187_Prg[0] = byte_data;
	  Map187_Set_CPU_Banks();
	}
	break;
      case	7:
	if( (Map187_ExtMode&0xA0)!=0xA0 ) {
	  Map187_Prg[1] = byte_data;
	  Map187_Set_CPU_Banks();
	}
	break;
      default:
	break;
      }
    } else {
      switch( Map187_ChrMode ) {
      case	0x2A:
	Map187_Prg[1] = 0x0F;
	break;
      case	0x28:
	Map187_Prg[2] = 0x17;
	break;
      case	0x26:
	break;
      default:
	break;
      }
      Map187_Set_CPU_Banks();
    }
    Map187_Bank[Map187_ChrMode&7] = byte_data;
    break;
    
  case	0xA000:
    if( byte_data & 0x01 ) {
      InfoNES_Mirroring( 0 );
    } else {
      InfoNES_Mirroring( 1 );
    }
    break;
  case	0xA001:
    break;
    
  case	0xC000:
    Map187_IRQ_Counter = byte_data;
    Map187_IRQ_Occur = 0;
    break;
  case	0xC001:
    Map187_IRQ_Latch = byte_data;
    Map187_IRQ_Occur = 0;
    break;
  case	0xE000:
  case	0xE002:
    Map187_IRQ_Enable = 0;
    Map187_IRQ_Occur = 0;
    break;
  case	0xE001:
  case	0xE003:
    Map187_IRQ_Enable = 1;
    Map187_IRQ_Occur = 0;
    break;
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 187 Write to APU Function  映射器 187：写 APU 函数        */
/*-------------------------------------------------------------------*/
void Map187_Apu( uint16_t address, uint8_t byte_data )
{
  Map187_LastWrite = byte_data;
  if( address == 0x5000 ) {
    Map187_ExtMode = byte_data;
    if( byte_data & 0x80 ) {
      if( byte_data & 0x20 ) {
	Map187_Prg[0] = ((byte_data&0x1E)<<1)+0;
	Map187_Prg[1] = ((byte_data&0x1E)<<1)+1;
	Map187_Prg[2] = ((byte_data&0x1E)<<1)+2;
	Map187_Prg[3] = ((byte_data&0x1E)<<1)+3;
      } else {
	Map187_Prg[2] = ((byte_data&0x1F)<<1)+0;
	Map187_Prg[3] = ((byte_data&0x1F)<<1)+1;
      }
    } else {
      Map187_Prg[0] = Map187_Bank[6];
      Map187_Prg[1] = Map187_Bank[7];
      Map187_Prg[2] = (NesHeader.rom_size<<1)-2;
      Map187_Prg[3] = (NesHeader.rom_size<<1)-1;
    }
    Map187_Set_CPU_Banks();
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 187 Read from APU Function  映射器 187：读 APU 函数       */
/*-------------------------------------------------------------------*/
uint8_t Map187_ReadApu( uint16_t address )
{
  switch( Map187_LastWrite&0x03 ) {
  case 0:
    return 0x83;
  case 1:
    return 0x83;
  case 2:
    return 0x42;
  case 3:
    return 0x00;
  }
  return 0;
}

/*-------------------------------------------------------------------*/
/*  Mapper 187 H-Sync Function  映射器 187：行同步函数               */
/*-------------------------------------------------------------------*/
void Map187_HSync()
{
  if( (PPU_Scanline >= 0 && PPU_Scanline <= 239) ) {
    if( PPU_R1 & R1_SHOW_SCR || PPU_R1 & R1_SHOW_SP ) {
      if( Map187_IRQ_Enable ) {
	if( !Map187_IRQ_Counter ) {
	  Map187_IRQ_Counter--;
	  Map187_IRQ_Enable = 0;
	  Map187_IRQ_Occur = 0xFF;
	} else {
	  Map187_IRQ_Counter--;
	}
      }
    }
  }
  if ( Map187_IRQ_Occur ) {
	  IRQ_REQ;
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 187 Set CPU Banks Function  映射器 187：设置 CPU 银行函数 */
/*-------------------------------------------------------------------*/
void Map187_Set_CPU_Banks()
{
  ROMBANK0 = ROMPAGE(((Map187_Prg[0]<<2)+0) % (NesHeader.rom_size<<1));
  ROMBANK1 = ROMPAGE(((Map187_Prg[1]<<2)+1) % (NesHeader.rom_size<<1));
  ROMBANK2 = ROMPAGE(((Map187_Prg[2]<<2)+2) % (NesHeader.rom_size<<1));
  ROMBANK3 = ROMPAGE(((Map187_Prg[3]<<2)+3) % (NesHeader.rom_size<<1));
}

/*-------------------------------------------------------------------*/
/*  Mapper 187 Set PPU Banks Function  映射器 187：设置 PPU 银行函数 */
/*-------------------------------------------------------------------*/
void Map187_Set_PPU_Banks()
{
  PPUBANK[ 0 ] = VROMPAGE(((Map187_Chr[0]<<3)+0) % (NesHeader.v_rom_size<<3));
  PPUBANK[ 1 ] = VROMPAGE(((Map187_Chr[1]<<3)+1) % (NesHeader.v_rom_size<<3));
  PPUBANK[ 2 ] = VROMPAGE(((Map187_Chr[2]<<3)+2) % (NesHeader.v_rom_size<<3));
  PPUBANK[ 3 ] = VROMPAGE(((Map187_Chr[3]<<3)+3) % (NesHeader.v_rom_size<<3));
  PPUBANK[ 4 ] = VROMPAGE(((Map187_Chr[4]<<3)+4) % (NesHeader.v_rom_size<<3));
  PPUBANK[ 5 ] = VROMPAGE(((Map187_Chr[5]<<3)+5) % (NesHeader.v_rom_size<<3));
  PPUBANK[ 6 ] = VROMPAGE(((Map187_Chr[6]<<3)+6) % (NesHeader.v_rom_size<<3));
  PPUBANK[ 7 ] = VROMPAGE(((Map187_Chr[7]<<3)+7) % (NesHeader.v_rom_size<<3));
  InfoNES_SetupChr();
}
