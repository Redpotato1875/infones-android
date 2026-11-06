/*===================================================================*/
/*                                                                   */
/*                   Mapper 100 : Nestile MMC 3  映射器 100：Nestile MMC 3 */
/*                                                                   */
/*===================================================================*/

uint8_t	Map100_Reg[8];
uint8_t	Map100_Prg0, Map100_Prg1, Map100_Prg2, Map100_Prg3;
uint8_t	Map100_Chr0, Map100_Chr1, Map100_Chr2, Map100_Chr3; 
uint8_t	Map100_Chr4, Map100_Chr5, Map100_Chr6, Map100_Chr7;

uint8_t	Map100_IRQ_Enable;
uint8_t	Map100_IRQ_Cnt;
uint8_t	Map100_IRQ_Latch;

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 100  初始化映射器 100                          */
/*-------------------------------------------------------------------*/
void Map100_Init()
{
  /* Initialize Mapper  初始化映射器 */
  MapperInit = Map100_Init;

  /* Write to Mapper  写入映射器 */
  MapperWrite = Map100_Write;

  /* Write to SRAM  写入 SRAM */
  MapperSram = Map0_Sram;

  /* Write to APU  写入 APU */
  MapperApu = Map0_Apu;

  /* Read from APU  从 APU 读取 */
  MapperReadApu = Map0_ReadApu;

  /* Callback at VSync  场同步回调 */
  MapperVSync = Map0_VSync;

  /* Callback at HSync  行同步回调 */
  MapperHSync = Map100_HSync;

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

  /* Set PPU Banks  设置 PPU 银行 */
  if ( NesHeader.v_rom_size > 0 )
  {
    Map100_Chr0 = 0;
    Map100_Chr1 = 1;
    Map100_Chr2 = 2;
    Map100_Chr3 = 3;
    Map100_Chr4 = 4;
    Map100_Chr5 = 5;
    Map100_Chr6 = 6;
    Map100_Chr7 = 7;
    Map100_Set_PPU_Banks();
  } else {
    Map100_Chr0 = Map100_Chr2 = Map100_Chr4 = Map100_Chr5 = Map100_Chr6 = Map100_Chr7 = 0;
    Map100_Chr1 = Map100_Chr3 = 1;
  }

  /* Set IRQ Registers  设置 IRQ 寄存器 */
  Map100_IRQ_Enable = 0;
  Map100_IRQ_Cnt = 0;
  Map100_IRQ_Latch = 0;
  for( int i = 0; i < 8; i++ ) { Map100_Reg[ i ] = 0x00; }

  /* Set up wiring of the interrupt pin  设置中断引脚接线方式 */
  K6502_Set_Int_Wiring( 1, 1 ); 
}

/*-------------------------------------------------------------------*/
/*  Mapper 100 Write Function  映射器 100：写函数                    */
/*-------------------------------------------------------------------*/
void Map100_Write( uint16_t address, uint8_t byte_data )
{
  switch ( address & 0xE001 )
  {
    case 0x8000:
      Map100_Reg[0] = byte_data;
      break;

    case 0x8001:
      Map100_Reg[1] = byte_data;

      switch ( Map100_Reg[0] & 0xC7 ) {
        case 0x00:
	  if ( NesHeader.v_rom_size > 0 ) {
	    Map100_Chr0 = byte_data&0xFE;
	    Map100_Chr1 = Map100_Chr0+1;
	    Map100_Set_PPU_Banks();
	  }
	  break;
	  
        case 0x01:
	  if ( NesHeader.v_rom_size > 0 ) {
	    Map100_Chr2 = byte_data&0xFE;
	    Map100_Chr3 = Map100_Chr2+1;
	    Map100_Set_PPU_Banks();
	  }
	  break;

        case 0x02:
	  if ( NesHeader.v_rom_size > 0 ) {
	    Map100_Chr4 = byte_data;
	    Map100_Set_PPU_Banks();
	  }
	  break;

        case 0x03:
	  if ( NesHeader.v_rom_size > 0 ) {
	    Map100_Chr5 = byte_data;
	    Map100_Set_PPU_Banks();
	  }
	  break;

        case 0x04:
	  if ( NesHeader.v_rom_size > 0 ) {
	    Map100_Chr6 = byte_data;
	    Map100_Set_PPU_Banks();
	  }
	  break;

        case 0x05:
	  if ( NesHeader.v_rom_size > 0 ) {
	    Map100_Chr7 = byte_data;
	    Map100_Set_PPU_Banks();
	  }
	  break;

        case 0x06:
	  Map100_Prg0 = byte_data;
	  Map100_Set_CPU_Banks();
	  break;

        case 0x07:
	  Map100_Prg1 = byte_data;
	  Map100_Set_CPU_Banks();
	  break;

        case 0x46:
	  Map100_Prg2 = byte_data;
	  Map100_Set_CPU_Banks();
	  break;

        case 0x47:
	  Map100_Prg3 = byte_data;
	  Map100_Set_CPU_Banks();
	  break;

        case 0x80:
	  if ( NesHeader.v_rom_size > 0 ) {
	    Map100_Chr4 = byte_data&0xFE;
	    Map100_Chr5 = Map100_Chr4+1;
	    Map100_Set_PPU_Banks();
	  }
	  break;

        case 0x81:
	  if ( NesHeader.v_rom_size > 0 ) {
	    Map100_Chr6 = byte_data&0xFE;
	    Map100_Chr7 = Map100_Chr6+1;
	    Map100_Set_PPU_Banks();
	  }
	  break;

        case 0x82:
	  if ( NesHeader.v_rom_size > 0 ) {
	    Map100_Chr0 = byte_data;
	    Map100_Set_PPU_Banks();
	  }
	  break;

        case 0x83:
	  if ( NesHeader.v_rom_size > 0 ) {
	    Map100_Chr1 = byte_data;
	    Map100_Set_PPU_Banks();
	  }
	  break;

        case 0x84:
	  if ( NesHeader.v_rom_size > 0 ) {
	    Map100_Chr2 = byte_data;
	    Map100_Set_PPU_Banks();
	  }
	  break;

        case 0x85:
	  if( NesHeader.v_rom_size > 0 ) {
	    Map100_Chr3 = byte_data;
	    Map100_Set_PPU_Banks();
	  }
	  break;

      }
      break;

    case 0xA000:
      Map100_Reg[2] = byte_data;
      if ( !ROM_FourScr )
      {
	if( byte_data & 0x01 ) InfoNES_Mirroring( 0 );
	else		    InfoNES_Mirroring( 1 );
      }
      break;

    case 0xA001:
      Map100_Reg[3] = byte_data;
      break;

    case 0xC000:
      Map100_Reg[4] = byte_data;
      Map100_IRQ_Cnt = byte_data;
      break;

    case 0xC001:
      Map100_Reg[5] = byte_data;
      Map100_IRQ_Latch = byte_data;
      break;

    case 0xE000:
      Map100_Reg[6] = byte_data;
      Map100_IRQ_Enable = 0;
      break;

    case 0xE001:
      Map100_Reg[7] = byte_data;
      Map100_IRQ_Enable = 0xFF;
      break;
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 100 H-Sync Function  映射器 100：行同步函数               */
/*-------------------------------------------------------------------*/
void Map100_HSync()
{
  if( (PPU_Scanline >= 0 && PPU_Scanline <= 239) ) {
    if ( PPU_R1 & R1_SHOW_SCR || PPU_R1 & R1_SHOW_SP ) {
      if( Map100_IRQ_Enable ) {
	if( !(Map100_IRQ_Cnt--) ) {
	  Map100_IRQ_Cnt = Map100_IRQ_Latch;
	  IRQ_REQ;
	}
      }
    }
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 100 Set CPU Banks Function  映射器 100：设置 CPU 银行函数 */
/*-------------------------------------------------------------------*/
void Map100_Set_CPU_Banks()
{
  ROMBANK0 = ROMPAGE( Map100_Prg0 % ( NesHeader.rom_size << 1 ) );
  ROMBANK1 = ROMPAGE( Map100_Prg1 % ( NesHeader.rom_size << 1 ) );
  ROMBANK2 = ROMPAGE( Map100_Prg2 % ( NesHeader.rom_size << 1 ) );
  ROMBANK3 = ROMPAGE( Map100_Prg3 % ( NesHeader.rom_size << 1 ) );
}

/*-------------------------------------------------------------------*/
/*  Mapper 100 Set PPU Banks Function  映射器 100：设置 PPU 银行函数 */
/*-------------------------------------------------------------------*/
void Map100_Set_PPU_Banks()
{
  if ( NesHeader.v_rom_size > 0 )
  {
      PPUBANK[ 0 ] = VROMPAGE( Map100_Chr0 % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 1 ] = VROMPAGE( Map100_Chr1 % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 2 ] = VROMPAGE( Map100_Chr2 % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 3 ] = VROMPAGE( Map100_Chr3 % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 4 ] = VROMPAGE( Map100_Chr4 % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 5 ] = VROMPAGE( Map100_Chr5 % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 6 ] = VROMPAGE( Map100_Chr6 % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 7 ] = VROMPAGE( Map100_Chr7 % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();
  }
}

