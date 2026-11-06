/*===================================================================*/
/*                                                                   */
/*               Mapper 245 : Yong Zhe Dou E Long  映射器 245：勇者斗恶龙 */
/*                                                                   */
/*===================================================================*/

uint8_t	Map245_Reg[8];
uint8_t	Map245_Prg0, Map245_Prg1;
uint8_t	Map245_Chr01, Map245_Chr23, Map245_Chr4, Map245_Chr5, Map245_Chr6, Map245_Chr7;
uint8_t	Map245_WeSram;

uint8_t	Map245_IRQ_Enable;
uint8_t	Map245_IRQ_Counter;
uint8_t	Map245_IRQ_Latch;
uint8_t	Map245_IRQ_Request;

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 245  初始化映射器 245                          */
/*-------------------------------------------------------------------*/
void Map245_Init()
{
  /* Initialize Mapper  初始化映射器 */
  MapperInit = Map245_Init;

  /* Write to Mapper  写入映射器 */
  MapperWrite = Map245_Write;

  /* Write to SRAM  写入 SRAM */
  MapperSram = Map0_Sram;

  /* Write to APU  写入 APU */
  MapperApu = Map0_Apu;

  /* Read from APU  从 APU 读取 */
  MapperReadApu = Map0_ReadApu;

  /* Callback at VSync  场同步回调 */
  MapperVSync = Map0_VSync;

  /* Callback at HSync  行同步回调 */
  MapperHSync = Map245_HSync;

  /* Callback at PPU  PPU 回调 */
  MapperPPU = Map0_PPU;

  /* Callback at Rendering Screen ( 1:BG, 0:Sprite )  渲染画面回调（1：背景，0：精灵） */
  MapperRenderScreen = Map0_RenderScreen;

  /* Set SRAM Banks  设置 SRAM 银行 */
  SRAMBANK = SRAM;

  /* Set Registers  设置寄存器 */
  for( int i = 0; i < 8; i++ ) {
    Map245_Reg[i] = 0x00;
  }

  Map245_Prg0 = 0;
  Map245_Prg1 = 1;

  /* Set ROM Banks  设置 ROM 银行 */
  ROMBANK0 = ROMPAGE( 0 );
  ROMBANK1 = ROMPAGE( 1 );
  ROMBANK2 = ROMLASTPAGE( 1 );
  ROMBANK3 = ROMLASTPAGE( 0 );

  /* Set PPU Banks  设置 PPU 银行 */
  if ( NesHeader.v_rom_size > 0 ) {
    for ( int page_number = 0; page_number < 8; ++page_number )
      PPUBANK[ page_number ] = VROMPAGE( page_number );
    InfoNES_SetupChr();
  }

  Map245_WeSram  = 0;		// Disable  禁止
  Map245_IRQ_Enable = 0;	// Disable  禁止
  Map245_IRQ_Counter = 0;
  Map245_IRQ_Latch = 0;
  Map245_IRQ_Request = 0;

  /* Set up wiring of the interrupt pin  设置中断引脚接线方式 */
  K6502_Set_Int_Wiring( 1, 1 ); 
}

/*-------------------------------------------------------------------*/
/*  Mapper 245 Write Function  映射器 245：写函数                    */
/*-------------------------------------------------------------------*/
void Map245_Write( uint16_t address, uint8_t byte_data )
{
  switch( address&0xF7FF ) {
  case	0x8000:
    Map245_Reg[0] = byte_data;
    break;
  case	0x8001:
    Map245_Reg[1] = byte_data;
    switch( Map245_Reg[0] ) {
    case	0x00:
      Map245_Reg[3]=(byte_data & 2 )<<5;
      ROMBANK2 = ROMPAGE((0x3E|Map245_Reg[3]) % (NesHeader.rom_size<<1));
      ROMBANK3 = ROMPAGE((0x3F|Map245_Reg[3]) % (NesHeader.rom_size<<1));
      break;
    case	0x06:
      Map245_Prg0=byte_data;
      break;
    case	0x07:
      Map245_Prg1=byte_data;
      break;
    }
    ROMBANK0 = ROMPAGE((Map245_Prg0|Map245_Reg[3]) % (NesHeader.rom_size<<1));
    ROMBANK1 = ROMPAGE((Map245_Prg1|Map245_Reg[3]) % (NesHeader.rom_size<<1));
    break;
  case	0xA000:
    Map245_Reg[2] = byte_data;
    if( !ROM_FourScr ) {
      if( byte_data & 0x01 ) InfoNES_Mirroring( 0 );
      else		  InfoNES_Mirroring( 1 );
    }
    break;
  case	0xA001:
    
    break;
  case	0xC000:
    Map245_Reg[4] = byte_data;
    Map245_IRQ_Counter = byte_data;
    Map245_IRQ_Request = 0;
    break;
  case	0xC001:
    Map245_Reg[5] = byte_data;
    Map245_IRQ_Latch = byte_data;
    Map245_IRQ_Request = 0;
    break;
  case	0xE000:
    Map245_Reg[6] = byte_data;
    Map245_IRQ_Enable = 0;
    Map245_IRQ_Request = 0;
    break;
  case	0xE001:
    Map245_Reg[7] = byte_data;
    Map245_IRQ_Enable = 1;
    Map245_IRQ_Request = 0;
    break;
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 245 H-Sync Function  映射器 245：行同步函数               */
/*-------------------------------------------------------------------*/
void Map245_HSync()
{
  if( (PPU_Scanline >= 0 && PPU_Scanline <= 239) ) {
    if( PPU_R1 & R1_SHOW_SCR || PPU_R1 & R1_SHOW_SP ) {
      if( Map245_IRQ_Enable && !Map245_IRQ_Request ) {
	if( PPU_Scanline == 0 ) {
	  if( Map245_IRQ_Counter ) {
	    Map245_IRQ_Counter--;
	  }
	}
	if( !(Map245_IRQ_Counter--) ) {
	  Map245_IRQ_Request = 0xFF;
	  Map245_IRQ_Counter = Map245_IRQ_Latch;
	}
      }
    }
  }
  if( Map245_IRQ_Request ) {
    IRQ_REQ;
  }
}

#if 0
/*-------------------------------------------------------------------*/
/*  Mapper 245 Set CPU Banks Function  映射器 245：设置 CPU 银行函数 */
/*-------------------------------------------------------------------*/
void SetBank_CPU()
{
  ROMBANK0 = ROMPAGE( Map245_Prg0 % ( NesHeader.rom_size << 1 ) );
  ROMBANK1 = ROMPAGE( Map245_Prg1 % ( NesHeader.rom_size << 1 ) );
  ROMBANK2 = ROMLASTPAGE( 1 );
  ROMBANK3 = ROMLASTPAGE( 0 );
}

/*-------------------------------------------------------------------*/
/*  Mapper 245 Set PPU Banks Function  映射器 245：设置 PPU 银行函数 */
/*-------------------------------------------------------------------*/
void SetBank_PPU()
{
  if( NesHeader.v_rom_size > 0 ) {
    if( Map245_Reg[0] & 0x80 ) {
      PPUBANK[ 0 ] = VROMPAGE( Map245_Chr4 % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 1 ] = VROMPAGE( Map245_Chr5 % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 2 ] = VROMPAGE( Map245_Chr6 % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 3 ] = VROMPAGE( Map245_Chr7 % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 4 ] = VROMPAGE( ( Map245_Chr01 + 0 ) % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 5 ] = VROMPAGE( ( Map245_Chr01 + 1 ) % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 6 ] = VROMPAGE( ( Map245_Chr23 + 0 ) % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 7 ] = VROMPAGE( ( Map245_Chr23 + 1 ) % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();
    } else {
      PPUBANK[ 0 ] = VROMPAGE( ( Map245_Chr01 + 0 ) % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 1 ] = VROMPAGE( ( Map245_Chr01 + 1 ) % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 2 ] = VROMPAGE( ( Map245_Chr23 + 0 ) % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 3 ] = VROMPAGE( ( Map245_Chr23 + 1 ) % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 4 ] = VROMPAGE( Map245_Chr4 % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 5 ] = VROMPAGE( Map245_Chr5 % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 6 ] = VROMPAGE( Map245_Chr6 % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 7 ] = VROMPAGE( Map245_Chr7 % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();
    }
  } else {
    if( Map245_Reg[0] & 0x80 ) {
      PPUBANK[ 4 ] = CRAMPAGE( (Map245_Chr01+0)&0x07 );
      PPUBANK[ 5 ] = CRAMPAGE( (Map245_Chr01+1)&0x07 );
      PPUBANK[ 6 ] = CRAMPAGE( (Map245_Chr23+0)&0x07 );
      PPUBANK[ 7 ] = CRAMPAGE( (Map245_Chr23+1)&0x07 );
      PPUBANK[ 0 ] = CRAMPAGE( Map245_Chr4&0x07 );
      PPUBANK[ 1 ] = CRAMPAGE( Map245_Chr5&0x07 );
      PPUBANK[ 2 ] = CRAMPAGE( Map245_Chr6&0x07 );
      PPUBANK[ 3 ] = CRAMPAGE( Map245_Chr7&0x07 );
      InfoNES_SetupChr();
    } else {
      PPUBANK[ 0 ] = CRAMPAGE( (Map245_Chr01+0)&0x07 );
      PPUBANK[ 1 ] = CRAMPAGE( (Map245_Chr01+1)&0x07 );
      PPUBANK[ 2 ] = CRAMPAGE( (Map245_Chr23+0)&0x07 );
      PPUBANK[ 3 ] = CRAMPAGE( (Map245_Chr23+1)&0x07 );
      PPUBANK[ 4 ] = CRAMPAGE( Map245_Chr4&0x07 );
      PPUBANK[ 5 ] = CRAMPAGE( Map245_Chr5&0x07 );
      PPUBANK[ 6 ] = CRAMPAGE( Map245_Chr6&0x07 );
      PPUBANK[ 7 ] = CRAMPAGE( Map245_Chr7&0x07 );
      InfoNES_SetupChr();
    }
  }
}
#endif
