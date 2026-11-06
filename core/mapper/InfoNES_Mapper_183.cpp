/*===================================================================*/
/*                                                                   */
/*                 Mapper 183 : Gimmick (Bootleg)  映射器 183：Gimmick（盗版卡） */
/*                                                                   */
/*===================================================================*/

uint8_t	Map183_Reg[8];
uint8_t	Map183_IRQ_Enable;
int	Map183_IRQ_Counter;

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 183  初始化映射器 183                          */
/*-------------------------------------------------------------------*/
void Map183_Init()
{
  /* Initialize Mapper  初始化映射器 */
  MapperInit = Map183_Init;

  /* Write to Mapper  写入映射器 */
  MapperWrite = Map183_Write;

  /* Write to SRAM  写入 SRAM */
  MapperSram = Map0_Sram;

  /* Write to APU  写入 APU */
  MapperApu = Map0_Apu;

  /* Read from APU  从 APU 读取 */
  MapperReadApu = Map0_ReadApu;

  /* Callback at VSync  场同步回调 */
  MapperVSync = Map0_VSync;

  /* Callback at HSync  行同步回调 */
  MapperHSync = Map183_HSync;

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
  if ( NesHeader.v_rom_size > 0 ) {
    for ( int page_number = 0; page_number < 8; ++page_number )
      PPUBANK[ page_number ] = VROMPAGE( page_number );
    InfoNES_SetupChr();
  }

  /* Initialize Registers  初始化寄存器 */
  for( int i = 0; i < 8; i++ ) {
    Map183_Reg[i] = i;
  }
  Map183_IRQ_Enable = 0;
  Map183_IRQ_Counter = 0;

  /* Set up wiring of the interrupt pin  设置中断引脚接线方式 */
  K6502_Set_Int_Wiring( 1, 1 ); 
}

/*-------------------------------------------------------------------*/
/*  Mapper 183 Write Function  映射器 183：写函数                    */
/*-------------------------------------------------------------------*/
void Map183_Write( uint16_t address, uint8_t byte_data )
{
  switch( address ) {
  case	0x8800:
    ROMBANK0 = ROMPAGE( byte_data % (NesHeader.rom_size << 1) );
    break;
  case	0xA800:
    ROMBANK1 = ROMPAGE( byte_data % (NesHeader.rom_size << 1) );
    break;
  case	0xA000:
    ROMBANK2 = ROMPAGE( byte_data % (NesHeader.rom_size << 1) );
    break;
    
  case	0xB000:
    Map183_Reg[0] = (Map183_Reg[0]&0xF0)|(byte_data&0x0F);
    PPUBANK[ 0 ] = VROMPAGE( Map183_Reg[0] % (NesHeader.v_rom_size << 3) );
    InfoNES_SetupChr();
    break;
  case	0xB004:
    Map183_Reg[0] = (Map183_Reg[0]&0x0F)|((byte_data&0x0F)<<4);
    PPUBANK[ 0 ] = VROMPAGE( Map183_Reg[0] % (NesHeader.v_rom_size << 3) );
    InfoNES_SetupChr();
    break;
  case	0xB008:
    Map183_Reg[1] = (Map183_Reg[1]&0xF0)|(byte_data&0x0F);
    PPUBANK[ 1 ] = VROMPAGE( Map183_Reg[1] % (NesHeader.v_rom_size << 3) );
    InfoNES_SetupChr();
    break;
  case	0xB00C:
    Map183_Reg[1] = (Map183_Reg[1]&0x0F)|((byte_data&0x0F)<<4);
    PPUBANK[ 1 ] = VROMPAGE( Map183_Reg[1] % (NesHeader.v_rom_size << 3) );
    InfoNES_SetupChr();
    break;
    
  case	0xC000:
    Map183_Reg[2] = (Map183_Reg[2]&0xF0)|(byte_data&0x0F);
    PPUBANK[ 2 ] = VROMPAGE( Map183_Reg[2] % (NesHeader.v_rom_size << 3) );
    InfoNES_SetupChr();
    break;
  case	0xC004:
    Map183_Reg[2] = (Map183_Reg[2]&0x0F)|((byte_data&0x0F)<<4);
    PPUBANK[ 2 ] = VROMPAGE( Map183_Reg[2] % (NesHeader.v_rom_size << 3) );
    InfoNES_SetupChr();
    break;
  case	0xC008:
    Map183_Reg[3] = (Map183_Reg[3]&0xF0)|(byte_data&0x0F);
    PPUBANK[ 3 ] = VROMPAGE( Map183_Reg[3] % (NesHeader.v_rom_size << 3) );
    InfoNES_SetupChr();
    break;
  case	0xC00C:
    Map183_Reg[3] = (Map183_Reg[3]&0x0F)|((byte_data&0x0F)<<4);
    PPUBANK[ 3 ] = VROMPAGE( Map183_Reg[3] % (NesHeader.v_rom_size << 3) );
    InfoNES_SetupChr();
    break;
    
  case	0xD000:
    Map183_Reg[4] = (Map183_Reg[4]&0xF0)|(byte_data&0x0F);
    PPUBANK[ 4 ] = VROMPAGE( Map183_Reg[4] % (NesHeader.v_rom_size << 3) );
    InfoNES_SetupChr();
    break;
  case	0xD004:
    Map183_Reg[4] = (Map183_Reg[4]&0x0F)|((byte_data&0x0F)<<4);
    PPUBANK[ 4 ] = VROMPAGE( Map183_Reg[4] % (NesHeader.v_rom_size << 3) );
    InfoNES_SetupChr();
    break;
  case	0xD008:
    Map183_Reg[5] = (Map183_Reg[5]&0xF0)|(byte_data&0x0F);
    PPUBANK[ 5 ] = VROMPAGE( Map183_Reg[5] % (NesHeader.v_rom_size << 3) );
    InfoNES_SetupChr();
    break;
  case	0xD00C:
    Map183_Reg[5] = (Map183_Reg[5]&0x0F)|((byte_data&0x0F)<<4);
    PPUBANK[ 5 ] = VROMPAGE( Map183_Reg[5] % (NesHeader.v_rom_size << 3) );
    InfoNES_SetupChr();
    break;
    
  case	0xE000:
    Map183_Reg[6] = (Map183_Reg[6]&0xF0)|(byte_data&0x0F);
    PPUBANK[ 6 ] = VROMPAGE( Map183_Reg[6] % (NesHeader.v_rom_size << 3) );
    InfoNES_SetupChr();
    break;
  case	0xE004:
    Map183_Reg[6] = (Map183_Reg[6]&0x0F)|((byte_data&0x0F)<<4);
    PPUBANK[ 6 ] = VROMPAGE( Map183_Reg[6] % (NesHeader.v_rom_size << 3) );
    InfoNES_SetupChr();
    break;
  case	0xE008:
    Map183_Reg[7] = (Map183_Reg[3]&0xF0)|(byte_data&0x0F);
    PPUBANK[ 7 ] = VROMPAGE( Map183_Reg[7] % (NesHeader.v_rom_size << 3) );
    InfoNES_SetupChr();
    break;
  case	0xE00C:
    Map183_Reg[7] = (Map183_Reg[3]&0x0F)|((byte_data&0x0F)<<4);
    PPUBANK[ 7 ] = VROMPAGE( Map183_Reg[7] % (NesHeader.v_rom_size << 3) );
    InfoNES_SetupChr();
    break;
    
  case	0x9008:
    if( byte_data == 1 ) {
      for( int i = 0; i < 8; i++ ) {
	Map183_Reg[i] = i;
      }
      /* Set ROM Banks  设置 ROM 银行 */
      ROMBANK0 = ROMPAGE( 0 );
      ROMBANK1 = ROMPAGE( 1 );
      ROMBANK2 = ROMLASTPAGE( 1 );
      ROMBANK3 = ROMLASTPAGE( 0 );

      /* Set PPU Banks  设置 PPU 银行 */
      if ( NesHeader.v_rom_size > 0 ) {
	PPUBANK[ 0 ] = VROMPAGE( 0 );
	PPUBANK[ 1 ] = VROMPAGE( 1 );
	PPUBANK[ 2 ] = VROMPAGE( 2 );
	PPUBANK[ 3 ] = VROMPAGE( 3 );
	PPUBANK[ 4 ] = VROMPAGE( 4 );
	PPUBANK[ 5 ] = VROMPAGE( 5 );
	PPUBANK[ 6 ] = VROMPAGE( 6 );
	PPUBANK[ 7 ] = VROMPAGE( 7 );
	InfoNES_SetupChr();
      }
    }
    break;
    
  case	0x9800:
    if( byte_data == 0 )      InfoNES_Mirroring( 1 );
    else if( byte_data == 1 ) InfoNES_Mirroring( 0 );
    else if( byte_data == 2 ) InfoNES_Mirroring( 2 );
    else if( byte_data == 3 ) InfoNES_Mirroring( 3 );
    break;
    
  case	0xF000:
    Map183_IRQ_Counter = (Map183_IRQ_Counter&0xFF00)|byte_data;
    break;
  case	0xF004:
    Map183_IRQ_Counter = (Map183_IRQ_Counter&0x00FF)|(byte_data<<8);
    break;
  case	0xF008:
    Map183_IRQ_Enable = byte_data;
    break;
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 183 H-Sync Function  映射器 183：行同步函数               */
/*-------------------------------------------------------------------*/
void Map183_HSync()
{
  if( Map183_IRQ_Enable & 0x02 ) {
    if( Map183_IRQ_Counter <= 113 ) {
      Map183_IRQ_Counter = 0;
      IRQ_REQ;
    } else {
      Map183_IRQ_Counter -= 113;
    }
  }
}
