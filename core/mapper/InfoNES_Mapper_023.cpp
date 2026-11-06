/*===================================================================*/
/*                                                                   */
/*                  Mapper 23 (Konami VRC2 type B)  映射器 23（Konami VRC2 type B） */
/*                                                                   */
/*===================================================================*/

uint8_t Map23_Regs[ 9 ];

uint8_t Map23_IRQ_Enable;
uint8_t Map23_IRQ_Cnt;
uint8_t Map23_IRQ_Latch;

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 23  初始化映射器 23                            */
/*-------------------------------------------------------------------*/
void Map23_Init()
{
  /* Initialize Mapper  初始化映射器 */
  MapperInit = Map23_Init;

  /* Write to Mapper  写入映射器 */
  MapperWrite = Map23_Write;

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

  /* Set ROM Banks  设置 ROM 银行 */
  ROMBANK0 = ROMPAGE( 0 );
  ROMBANK1 = ROMPAGE( 1 );
  ROMBANK2 = ROMLASTPAGE( 1 );
  ROMBANK3 = ROMLASTPAGE( 0 );

  /* Set PPU Banks  设置 PPU 银行 */
  if ( NesHeader.v_rom_size > 0 )
  {
    for ( int page_number = 0; page_number < 8; ++page_number )
      PPUBANK[ page_number ] = VROMPAGE( page_number );
    InfoNES_SetupChr();
  }

  /* Initialize State Flag  初始化状态标志 */
  Map23_Regs[ 0 ] = 0;
  Map23_Regs[ 1 ] = 1;
  Map23_Regs[ 2 ] = 2;
  Map23_Regs[ 3 ] = 3;
  Map23_Regs[ 4 ] = 4;
  Map23_Regs[ 5 ] = 5;
  Map23_Regs[ 6 ] = 6;
  Map23_Regs[ 7 ] = 7;
  Map23_Regs[ 8 ] = 0;

  Map23_IRQ_Enable = 0;
  Map23_IRQ_Cnt = 0;
  Map23_IRQ_Latch = 0;

  /* Set up wiring of the interrupt pin  设置中断引脚接线方式 */
  K6502_Set_Int_Wiring( 1, 1 ); 
}

/*-------------------------------------------------------------------*/
/*  Mapper 23 Write Function  映射器 23：写函数                      */
/*-------------------------------------------------------------------*/
void Map23_Write( uint16_t address, uint8_t byte_data )
{
  switch ( address )
  {
    case 0x8000:
    case 0x8004:
    case 0x8008:
    case 0x800c:
      byte_data %= ( NesHeader.rom_size << 1 );

      if ( Map23_Regs[ 8 ] )
      {
        ROMBANK2 = ROMPAGE( byte_data );
      } else {
        ROMBANK0 = ROMPAGE( byte_data );
      }
      break;

    case 0x9000:
      switch ( byte_data & 0x03 )
      {
       case 0x00:
          InfoNES_Mirroring( 1 );
          break;
        case 0x01:
          InfoNES_Mirroring( 0 ); 
          break;
        case 0x02:
          InfoNES_Mirroring( 3 );  
          break;
        case 0x03:
          InfoNES_Mirroring( 2 );  
          break;
      }
      break;

    case 0x9008:
      Map23_Regs[ 8 ] = byte_data & 0x02;
      break;

    case 0xa000:
    case 0xa004:
    case 0xa008:
    case 0xa00c:
      byte_data %= ( NesHeader.rom_size << 1 );
      ROMBANK1 = ROMPAGE( byte_data );
      break;

    case 0xb000:
      Map23_Regs[ 0 ] = ( Map23_Regs[ 0 ] & 0xf0 ) | ( byte_data & 0x0f );
      PPUBANK[ 0 ] = VROMPAGE( Map23_Regs[ 0 ] % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();
      break;

    case 0xb001:
    case 0xb004:
      Map23_Regs[ 0 ] = ( Map23_Regs[ 0 ] & 0x0f ) | ( ( byte_data & 0x0f ) << 4 );
      PPUBANK[ 0 ] = VROMPAGE( Map23_Regs[ 0 ] % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();
      break;

    case 0xb002:
    case 0xb008:
      Map23_Regs[ 1 ] = ( Map23_Regs[ 1 ] & 0xf0 ) | ( byte_data & 0x0f );
      PPUBANK[ 1 ] = VROMPAGE( Map23_Regs[ 1 ] % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();
      break;

    case 0xb003:
    case 0xb00c:
      Map23_Regs[ 1 ] = ( Map23_Regs[ 1 ] & 0x0f ) | ( ( byte_data & 0x0f ) << 4 );
      PPUBANK[ 1 ] = VROMPAGE( Map23_Regs[ 1 ] % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();
      break;

    case 0xc000:
      Map23_Regs[ 2 ] = ( Map23_Regs[ 2 ] & 0xf0 ) | ( byte_data & 0x0f );
      PPUBANK[ 2 ] = VROMPAGE( Map23_Regs[ 2 ] % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();
      break;

    case 0xc001:
    case 0xc004:
      Map23_Regs[ 2 ] = ( Map23_Regs[ 2 ] & 0x0f ) | ( ( byte_data & 0x0f ) << 4 );
      PPUBANK[ 2 ] = VROMPAGE( Map23_Regs[ 2 ] % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();
      break;

    case 0xc002:
    case 0xc008:
      Map23_Regs[ 3 ] = ( Map23_Regs[ 3 ] & 0xf0 ) | ( byte_data & 0x0f );
      PPUBANK[ 3 ] = VROMPAGE( Map23_Regs[ 3 ] % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();
      break;

    case 0xc003:
    case 0xc00c:
      Map23_Regs[ 3 ] = ( Map23_Regs[ 3 ] & 0x0f ) | ( ( byte_data & 0x0f ) << 4 );
      PPUBANK[ 3 ] = VROMPAGE( Map23_Regs[ 3 ] % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();
      break;

    case 0xd000:
      Map23_Regs[ 4 ] = ( Map23_Regs[ 4 ] & 0xf0 ) | ( byte_data & 0x0f );
      PPUBANK[ 4 ] = VROMPAGE( Map23_Regs[ 4 ] % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();
      break;

    case 0xd001:
    case 0xd004:
      Map23_Regs[ 4 ] = ( Map23_Regs[ 4 ] & 0x0f ) | ( ( byte_data & 0x0f ) << 4 );
      PPUBANK[ 4 ] = VROMPAGE( Map23_Regs[ 4 ] % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();
      break;

    case 0xd002:
    case 0xd008:
      Map23_Regs[ 5 ] = ( Map23_Regs[ 5 ] & 0xf0 ) | ( byte_data & 0x0f );
      PPUBANK[ 5 ] = VROMPAGE( Map23_Regs[ 5 ] % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();
      break;

    case 0xd003:
    case 0xd00c:
      Map23_Regs[ 5 ] = ( Map23_Regs[ 5 ] & 0x0f ) | ( ( byte_data & 0x0f ) << 4 );
      PPUBANK[ 5 ] = VROMPAGE( Map23_Regs[ 5 ] % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();
      break;   
      
    case 0xe000:
      Map23_Regs[ 6 ] = ( Map23_Regs[ 6 ] & 0xf0 ) | ( byte_data & 0x0f );
      PPUBANK[ 6 ] = VROMPAGE( Map23_Regs[ 6 ] % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();
      break;

    case 0xe001:
    case 0xe004:
      Map23_Regs[ 6 ] = ( Map23_Regs[ 6 ] & 0x0f ) | ( ( byte_data & 0x0f ) << 4 );
      PPUBANK[ 6 ] = VROMPAGE( Map23_Regs[ 6 ] % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();
      break;

    case 0xe002:
    case 0xe008:
      Map23_Regs[ 7 ] = ( Map23_Regs[ 7 ] & 0xf0 ) | ( byte_data & 0x0f );
      PPUBANK[ 7 ] = VROMPAGE( Map23_Regs[ 7 ] % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();
      break;

    case 0xe003:
    case 0xe00c:
      Map23_Regs[ 7 ] = ( Map23_Regs[ 7 ] & 0x0f ) | ( ( byte_data & 0x0f ) << 4 );
      PPUBANK[ 7 ] = VROMPAGE( Map23_Regs[ 7 ] % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();
      break;

    case 0xf000:
      Map23_IRQ_Latch = ( Map23_IRQ_Latch & 0xf0 ) | ( byte_data & 0x0f );
      break;

    case 0xf004:
      Map23_IRQ_Latch = ( Map23_IRQ_Latch & 0xf0 ) | ( ( byte_data & 0x0f ) << 4 );
      break;

    case 0xf008:
      Map23_IRQ_Enable = byte_data & 0x03;
      if ( Map23_IRQ_Enable & 0x02 )
      {
        Map23_IRQ_Cnt = Map23_IRQ_Latch;
      }
      break;

    case 0xf00c:
      if ( Map23_IRQ_Enable & 0x01 )
      {
        Map23_IRQ_Enable |= 0x02;
      } else {
        Map23_IRQ_Enable &= 0x01;
      }
      break;
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 23 H-Sync Function  映射器 23：行同步函数                 */
/*-------------------------------------------------------------------*/
void Map23_HSync()
{
/*
 *  Callback at HSync  行同步回调
 *
 */
  if ( Map23_IRQ_Enable & 0x02 )
  {
    if ( Map23_IRQ_Cnt == 0xff )
    {
      IRQ_REQ;

      Map23_IRQ_Cnt = Map23_IRQ_Latch;
      if ( Map23_IRQ_Enable & 0x01 )
      {
        Map23_IRQ_Enable |= 0x02;
      } else {
        Map23_IRQ_Enable &= 0x01;
      }
    } else {
      Map23_IRQ_Cnt++;
    }
  }
}
