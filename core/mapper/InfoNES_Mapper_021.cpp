/*===================================================================*/
/*                                                                   */
/*                   Mapper 21 (Konami VRC4 2A)  映射器 21（Konami VRC4 2A） */
/*                                                                   */
/*===================================================================*/

uint8_t Map21_Regs[ 10 ];
uint8_t Map21_IRQ_Enable;
uint8_t Map21_IRQ_Cnt;
uint8_t Map21_IRQ_Latch;

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 21  初始化映射器 21                            */
/*-------------------------------------------------------------------*/
void Map21_Init()
{
  /* Initialize Mapper  初始化映射器 */
  MapperInit = Map21_Init;

  /* Write to Mapper  写入映射器 */
  MapperWrite = Map21_Write;

  /* Write to SRAM  写入 SRAM */
  MapperSram = Map0_Sram;

  /* Write to APU  写入 APU */
  MapperApu = Map0_Apu;

  /* Read from APU  从 APU 读取 */
  MapperReadApu = Map0_ReadApu;

  /* Callback at VSync  场同步回调 */
  MapperVSync = Map0_VSync;

  /* Callback at HSync  行同步回调 */
  MapperHSync = Map21_HSync;

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

  /* Initialize State Registers  初始化状态寄存器 */
  for ( int page_number = 0; page_number < 8; page_number++ )
  {
    Map21_Regs[ page_number ] = page_number;
  }
  Map21_Regs[ 8 ] = 0;

  Map21_IRQ_Enable = 0;
  Map21_IRQ_Cnt = 0;
  Map21_IRQ_Latch = 0;

  /* Set up wiring of the interrupt pin  设置中断引脚接线方式 */
  K6502_Set_Int_Wiring( 1, 1 ); 
}

/*-------------------------------------------------------------------*/
/*  Mapper 21 Write Function  映射器 21：写函数                      */
/*-------------------------------------------------------------------*/
void Map21_Write( uint16_t address, uint8_t byte_data )
{
  switch ( address & 0xf00f )
  {
    /* Set ROM Banks  设置 ROM 银行 */
    case 0x8000:
      if ( Map21_Regs[ 8 ] & 0x02 )
      {
        byte_data %= ( NesHeader.rom_size << 1 );
        ROMBANK2 = ROMPAGE( byte_data );
      } else {
        byte_data %= ( NesHeader.rom_size << 1 );
        ROMBANK0 = ROMPAGE( byte_data );
      }
      break;

    case 0xa000:
      byte_data %= ( NesHeader.rom_size << 1 );
      ROMBANK1 = ROMPAGE( byte_data );
      break;

    /* Name Table Mirroring  命名表镜像 */
    case 0x9000:
      switch ( byte_data & 0x03 )
      {
        case 0:
          InfoNES_Mirroring( 1 );
          break;

        case 1:
          InfoNES_Mirroring( 0 );
          break;
       
        case 2:
          InfoNES_Mirroring( 3 );
          break;

        case 3:
          InfoNES_Mirroring( 2 );
          break; 
      }
      break;

    case 0x9002:
      Map21_Regs[ 8 ] = byte_data;
      break;

    case 0xb000:
      Map21_Regs[ 0 ] = ( Map21_Regs[ 0 ] & 0xf0 ) | ( byte_data & 0x0f );
      PPUBANK[ 0 ] = VROMPAGE( Map21_Regs[ 0 ] % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();
      break;

    case 0xb002:
      Map21_Regs[ 0 ] = ( Map21_Regs[ 0 ] & 0x0f ) | ( ( byte_data & 0x0f ) << 4 );
      PPUBANK[ 0 ] = VROMPAGE( Map21_Regs[ 0 ] % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();
      break;

    case 0xb001:
    case 0xb004:
      Map21_Regs[ 1 ] = ( Map21_Regs[ 1 ] & 0xf0 ) | ( byte_data & 0x0f );
      PPUBANK[ 1 ] = VROMPAGE( Map21_Regs[ 1 ] % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();
      break;

    case 0xb003:
    case 0xb006:
      Map21_Regs[ 1 ] = ( Map21_Regs[ 1 ] & 0x0f ) | ( ( byte_data & 0x0f ) << 4 );
      PPUBANK[ 1 ] = VROMPAGE( Map21_Regs[ 1 ] % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();
      break;

    case 0xc000:
      Map21_Regs[ 2 ] = ( Map21_Regs[ 2 ] & 0xf0 ) | ( byte_data & 0x0f );
      PPUBANK[ 2 ] = VROMPAGE( Map21_Regs[ 2 ] % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();
      break;

    case 0xc002:
      Map21_Regs[ 2 ] = ( Map21_Regs[ 2 ] & 0x0f ) | ( ( byte_data & 0x0f ) << 4 );
      PPUBANK[ 2 ] = VROMPAGE( Map21_Regs[ 2 ] % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();
      break;

    case 0xc001:
    case 0xc004:
      Map21_Regs[ 3 ] = ( Map21_Regs[ 3 ] & 0xf0 ) | ( byte_data & 0x0f );
      PPUBANK[ 3 ] = VROMPAGE( Map21_Regs[ 3 ] % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();
      break;

    case 0xc003:
    case 0xc006:
      Map21_Regs[ 3 ] = ( Map21_Regs[ 3 ] & 0x0f ) | ( ( byte_data & 0x0f ) << 4 );
      PPUBANK[ 3 ] = VROMPAGE( Map21_Regs[ 3 ] % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();
      break;

    case 0xd000:
      Map21_Regs[ 4 ] = ( Map21_Regs[ 4 ] & 0xf0 ) | ( byte_data & 0x0f );
      PPUBANK[ 4 ] = VROMPAGE( Map21_Regs[ 4 ] % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();
      break;

    case 0xd002:
      Map21_Regs[ 4 ] = ( Map21_Regs[ 4 ] & 0x0f ) | ( ( byte_data & 0x0f ) << 4 );
      PPUBANK[ 4 ] = VROMPAGE( Map21_Regs[ 4 ] % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();
      break;

    case 0xd001:
    case 0xd004:
      Map21_Regs[ 5 ] = ( Map21_Regs[ 5 ] & 0xf0 ) | ( byte_data & 0x0f );
      PPUBANK[ 5 ] = VROMPAGE( Map21_Regs[ 5 ] % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();
      break;

    case 0xd003:
    case 0xd006:
      Map21_Regs[ 5 ] = ( Map21_Regs[ 5 ] & 0x0f ) | ( ( byte_data & 0x0f ) << 4 );
      PPUBANK[ 5 ] = VROMPAGE( Map21_Regs[ 5 ] % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();
      break;

    case 0xe000:
      Map21_Regs[ 6 ] = ( Map21_Regs[ 6 ] & 0xf0 ) | ( byte_data & 0x0f );
      PPUBANK[ 6 ] = VROMPAGE( Map21_Regs[ 6 ] % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();
      break;

    case 0xe002:
      Map21_Regs[ 6 ] = ( Map21_Regs[ 6 ] & 0x0f ) | ( ( byte_data & 0x0f ) << 4 );
      PPUBANK[ 6 ] = VROMPAGE( Map21_Regs[ 6 ] % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();
      break;

    case 0xe001:
    case 0xe004:
      Map21_Regs[ 7 ] = ( Map21_Regs[ 7 ] & 0xf0 ) | ( byte_data & 0x0f );
      PPUBANK[ 7 ] = VROMPAGE( Map21_Regs[ 7 ] % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();
      break;

    case 0xe003:
    case 0xe006:
      Map21_Regs[ 7 ] = ( Map21_Regs[ 7 ] & 0x0f ) | ( ( byte_data & 0x0f ) << 4 );
      PPUBANK[ 7 ] = VROMPAGE( Map21_Regs[ 7 ] % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();
      break;

    case 0xf000:
      Map21_IRQ_Latch = ( Map21_IRQ_Latch & 0xf0 ) | ( byte_data & 0x0f );
      break;

    case 0xf002:
      Map21_IRQ_Latch = ( Map21_IRQ_Latch & 0x0f ) | ( ( byte_data & 0x0f ) << 4 );
      break;

    case 0xf003:
      if ( Map21_IRQ_Enable & 0x01 )
      {
        Map21_IRQ_Enable |= 0x02;
      } else {
        Map21_IRQ_Enable &= 0x01;
      }
      break;

    case 0xf004:
      Map21_IRQ_Enable = byte_data & 0x03;
      if ( Map21_IRQ_Enable & 0x02 )
      {
        Map21_IRQ_Cnt = Map21_IRQ_Latch;
      }
      break;
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 21 H-Sync Function  映射器 21：行同步函数                 */
/*-------------------------------------------------------------------*/
void Map21_HSync()
{
/*
 *  Callback at HSync  行同步回调
 *
 */
  if ( Map21_IRQ_Enable & 0x02 )
  {
    if ( Map21_IRQ_Cnt == 0xff )
    {
      Map21_IRQ_Cnt = Map21_IRQ_Latch;
      
      if ( Map21_IRQ_Enable & 0x01 )
      {
        Map21_IRQ_Enable |= 0x02;
      } else {
        Map21_IRQ_Enable &= 0x01;
      }
      IRQ_REQ;
    } else {
      Map21_IRQ_Cnt++;
    }
  }
}
