/*===================================================================*/
/*                                                                   */
/*                   Mapper 18 (Jaleco SS8806)  映射器 18（Jaleco SS8806） */
/*                                                                   */
/*===================================================================*/

uint8_t Map18_Regs[11];
uint8_t Map18_IRQ_Enable;
uint16_t Map18_IRQ_Latch;
uint16_t Map18_IRQ_Cnt;

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 18  初始化映射器 18                            */
/*-------------------------------------------------------------------*/
void Map18_Init()
{
  /* Initialize Mapper  初始化映射器 */
  MapperInit = Map18_Init;

  /* Write to Mapper  写入映射器 */
  MapperWrite = Map18_Write;

  /* Write to SRAM  写入 SRAM */
  MapperSram = Map0_Sram;

  /* Write to APU  写入 APU */
  MapperApu = Map0_Apu;

  /* Read from APU  从 APU 读取 */
  MapperReadApu = Map0_ReadApu;

  /* Callback at VSync  场同步回调 */
  MapperVSync = Map0_VSync;

  /* Callback at HSync  行同步回调 */
  MapperHSync = Map18_HSync;

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

  /* Initialize Regs  初始化寄存器 */
  for ( int i = 0; i < sizeof( Map18_Regs ); i++ )
  {
    Map18_Regs[ i ] = 0;
  }
  Map18_IRQ_Enable = 0;
  Map18_IRQ_Latch = 0;
  Map18_IRQ_Cnt = 0;

  /* Set up wiring of the interrupt pin  设置中断引脚接线方式 */
  K6502_Set_Int_Wiring( 1, 1 ); 
}

/*-------------------------------------------------------------------*/
/*  Mapper 18 Write Function  映射器 18：写函数                      */
/*-------------------------------------------------------------------*/
void Map18_Write( uint16_t address, uint8_t byte_data )
{
  switch( address )
  {
    /* Set ROM Banks  设置 ROM 银行 */
    case 0x8000:
      Map18_Regs[ 0 ] = ( Map18_Regs[ 0 ] & 0xf0 ) | ( byte_data & 0x0f );
      ROMBANK0 = ROMPAGE( Map18_Regs[ 0 ] % ( NesHeader.rom_size << 1 ) );
      break;

    case 0x8001:
      Map18_Regs[ 0 ] = ( Map18_Regs[ 0 ] & 0x0f ) | ( ( byte_data & 0x0f ) << 4 );
      ROMBANK0 = ROMPAGE( Map18_Regs[ 0 ] % ( NesHeader.rom_size << 1 ) );
      break;

    case 0x8002:
      Map18_Regs[ 1 ] = ( Map18_Regs[ 1 ] & 0xf0 ) | ( byte_data & 0x0f );
      ROMBANK1 = ROMPAGE( Map18_Regs[ 1 ] % ( NesHeader.rom_size << 1 ) );
      break;

    case 0x8003:
      Map18_Regs[ 1 ] = ( Map18_Regs[ 1 ] & 0x0f ) | ( ( byte_data & 0x0f ) << 4 );
      ROMBANK1 = ROMPAGE( Map18_Regs[ 1 ] % ( NesHeader.rom_size << 1 ) );
      break;

    case 0x9000:
      Map18_Regs[ 2 ] = ( Map18_Regs[ 2 ] & 0xf0 ) | ( byte_data & 0x0f );
      ROMBANK2 = ROMPAGE( Map18_Regs[ 2 ] % ( NesHeader.rom_size << 1 ) );
      break;

    case 0x9001:
      Map18_Regs[ 2 ] = ( Map18_Regs[ 2 ] & 0x0f ) | ( ( byte_data & 0x0f ) << 4 );
      ROMBANK2 = ROMPAGE( Map18_Regs[ 2 ] % ( NesHeader.rom_size << 1 ) );
      break;

    /* Set PPU Banks  设置 PPU 银行 */
    case 0xA000:
      Map18_Regs[ 3 ]  = ( Map18_Regs[ 3 ] & 0xf0 ) | ( byte_data & 0x0f );
      PPUBANK[ 0 ] = VROMPAGE( Map18_Regs[ 3 ] % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();      
      break;

    case 0xA001:
      Map18_Regs[ 3 ] = ( Map18_Regs[ 3 ] & 0x0f ) | ( ( byte_data & 0x0f ) << 4 );
      PPUBANK[ 0 ] = VROMPAGE( Map18_Regs[ 3 ] % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();        
      break;

    case 0xA002:
      Map18_Regs[ 4 ]  = ( Map18_Regs[ 4 ] & 0xf0 ) | ( byte_data & 0x0f );
      PPUBANK[ 1 ] = VROMPAGE( Map18_Regs[ 4 ] % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();         
      break;

    case 0xA003:
      Map18_Regs[ 4 ] = ( Map18_Regs[ 4 ] & 0x0f ) | ( ( byte_data & 0x0f ) << 4 );
      PPUBANK[ 1 ] = VROMPAGE( Map18_Regs[ 4 ] % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr(); 
      break;

    case 0xB000:
      Map18_Regs[ 5 ]  = ( Map18_Regs[ 5 ] & 0xf0 ) | ( byte_data & 0x0f );
      PPUBANK[ 2 ] = VROMPAGE( Map18_Regs[ 5 ] % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();      
      break;

    case 0xB001:
      Map18_Regs[ 5 ] = ( Map18_Regs[ 5 ] &0x0f ) | ( ( byte_data & 0x0f ) << 4 );
      PPUBANK[ 2 ] = VROMPAGE( Map18_Regs[ 5 ] % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();        
      break;

    case 0xB002:
      Map18_Regs[ 6 ]  = ( Map18_Regs[ 6 ] & 0xf0 ) | ( byte_data & 0x0f );
      PPUBANK[ 3 ] = VROMPAGE( Map18_Regs[ 6 ] % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();         
      break;

    case 0xB003:
      Map18_Regs[ 6 ] = ( Map18_Regs[ 6 ] & 0x0f ) | ( ( byte_data & 0x0f ) << 4 );
      PPUBANK[ 3 ] = VROMPAGE( Map18_Regs[ 6 ] % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr(); 
      break;

    case 0xC000:
      Map18_Regs[ 7 ]  = ( Map18_Regs[ 7 ] & 0xf0 ) | ( byte_data & 0x0f );
      PPUBANK[ 4 ] = VROMPAGE( Map18_Regs[ 7 ] % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();      
      break;

    case 0xC001:
      Map18_Regs[ 7 ] = ( Map18_Regs[ 7 ] & 0x0f ) | ( ( byte_data & 0x0f ) << 4 );
      PPUBANK[ 4 ] = VROMPAGE( Map18_Regs[ 7 ] % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();        
      break;

    case 0xC002:
      Map18_Regs[ 8 ]  = ( Map18_Regs[ 8 ] & 0xf0 ) | ( byte_data & 0x0f );
      PPUBANK[ 5 ] = VROMPAGE( Map18_Regs[ 8 ] % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();         
      break;

    case 0xC003:
      Map18_Regs[ 8 ] = ( Map18_Regs[ 8 ] & 0x0f ) | ( ( byte_data & 0x0f ) << 4 );
      PPUBANK[ 5 ] = VROMPAGE( Map18_Regs[ 8 ] % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr(); 
      break;

    case 0xD000:
      Map18_Regs[ 9 ]  = ( Map18_Regs[ 9 ] & 0xf0 ) | ( byte_data & 0x0f );
      PPUBANK[ 6 ] = VROMPAGE( Map18_Regs[ 9 ] % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();      
      break;

    case 0xD001:
      Map18_Regs[ 9 ] = ( Map18_Regs[ 9 ] & 0x0f ) | ( ( byte_data & 0x0f ) << 4 );
      PPUBANK[ 6 ] = VROMPAGE( Map18_Regs[ 9 ] % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();        
      break;

    case 0xD002:
      Map18_Regs[ 10 ]  = ( Map18_Regs[ 10 ] & 0xf0 ) | ( byte_data & 0x0f );
      PPUBANK[ 7 ] = VROMPAGE( Map18_Regs[ 10 ] % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();         
      break;

    case 0xD003:
      Map18_Regs[ 10 ] = ( Map18_Regs[ 10 ] & 0x0f ) | ( ( byte_data & 0x0f ) << 4 );
      PPUBANK[ 7 ] = VROMPAGE( Map18_Regs[ 10 ] % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr(); 
      break;

    case 0xE000:
      Map18_IRQ_Latch = ( Map18_IRQ_Latch & 0xfff0 ) | ( byte_data & 0x0f );
      break;

    case 0xE001:
      Map18_IRQ_Latch = ( Map18_IRQ_Latch & 0xff0f ) | ( ( byte_data & 0x0f ) << 4 );
      break;

    case 0xE002:
      Map18_IRQ_Latch = ( Map18_IRQ_Latch & 0xf0ff ) | ( ( byte_data & 0x0f ) << 8 );
      break;

    case 0xE003:
      Map18_IRQ_Latch = ( Map18_IRQ_Latch & 0x0fff ) | ( ( byte_data & 0x0f ) << 12 );
      break;

    case 0xF000:
      if ( ( byte_data & 0x01 ) == 0 )
      {
        Map18_IRQ_Cnt = 0;
      } else {
        Map18_IRQ_Cnt = Map18_IRQ_Latch;
      }
      break;

    case 0xF001:
      Map18_IRQ_Enable = byte_data & 0x01;
      break;

    /* Name Table Mirroring  命名表镜像 */
    case 0xF002:
      switch ( byte_data & 0x03 )
      {
        case 0:
          InfoNES_Mirroring( 0 );   /* Horizontal  水平 */
          break;
        case 1:
          InfoNES_Mirroring( 1 );   /* Vertical  垂直 */            
          break;
        case 2:
          InfoNES_Mirroring( 3 );   /* One Screen 0x2000  单屏 0x2000 */
          break;
      }    
      break;
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 18 H-Sync Function  映射器 18：行同步函数                 */
/*-------------------------------------------------------------------*/
void Map18_HSync()
{
/*
 *  Callback at HSync  行同步回调
 *
 */
  if ( Map18_IRQ_Enable )
  {
    if ( Map18_IRQ_Cnt <= 113 )
    {
      IRQ_REQ;
      Map18_IRQ_Cnt = 0;
      Map18_IRQ_Enable = 0;
    } else {
      Map18_IRQ_Cnt -= 113;
    }
  }
}
