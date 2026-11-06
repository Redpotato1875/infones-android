/*===================================================================*/
/*                                                                   */
/*                       Mapper 49 (Nin1)  映射器 49（Nin1）         */
/*                                                                   */
/*===================================================================*/

uint8_t  Map49_Regs[ 3 ];
uint32_t Map49_Prg0, Map49_Prg1;
uint32_t Map49_Chr01, Map49_Chr23;
uint32_t Map49_Chr4, Map49_Chr5, Map49_Chr6, Map49_Chr7;

uint8_t Map49_IRQ_Enable;
uint8_t Map49_IRQ_Cnt;
uint8_t Map49_IRQ_Latch;

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 49  初始化映射器 49                            */
/*-------------------------------------------------------------------*/
void Map49_Init()
{
  /* Initialize Mapper  初始化映射器 */
  MapperInit = Map49_Init;

  /* Write to Mapper  写入映射器 */
  MapperWrite = Map49_Write;

  /* Write to SRAM  写入 SRAM */
  MapperSram = Map49_Sram;

  /* Write to APU  写入 APU */
  MapperApu = Map0_Apu;

  /* Read from APU  从 APU 读取 */
  MapperReadApu = Map0_ReadApu;

  /* Callback at VSync  场同步回调 */
  MapperVSync = Map0_VSync;

  /* Callback at HSync  行同步回调 */
  MapperHSync = Map49_HSync;

  /* Callback at PPU  PPU 回调 */
  MapperPPU = Map0_PPU;

  /* Callback at Rendering Screen ( 1:BG, 0:Sprite )  渲染画面回调（1：背景，0：精灵） */
  MapperRenderScreen = Map0_RenderScreen;

  /* Set SRAM Banks  设置 SRAM 银行 */
  SRAMBANK = SRAM;

  /* Set ROM Banks  设置 ROM 银行 */
  Map49_Prg0 = 0;
  Map49_Prg1 = 1;
  ROMBANK0 = ROMPAGE( 0 );
  ROMBANK1 = ROMPAGE( 1 );
  ROMBANK2 = ROMPAGE( 2 );
  ROMBANK3 = ROMPAGE( 3 );
  
  /* Set PPU Banks  设置 PPU 银行 */
  Map49_Chr01 = 0;
  Map49_Chr23 = 2;
  Map49_Chr4  = 4;
  Map49_Chr5  = 5;
  Map49_Chr6  = 6;
  Map49_Chr7  = 7;

  for ( int page_number = 0; page_number < 8; ++page_number )
  {
    PPUBANK[ page_number ] = VROMPAGE( page_number );
  }
  InfoNES_SetupChr();
  
  /* Initialize IRQ Registers  初始化 IRQ 寄存器 */
  Map49_Regs[ 0 ] = Map49_Regs[ 1 ] = Map49_Regs[ 2 ] = 0;
  Map49_IRQ_Enable = 0;
  Map49_IRQ_Cnt = 0;
  Map49_IRQ_Latch = 0;

  /* Set up wiring of the interrupt pin  设置中断引脚接线方式 */
  K6502_Set_Int_Wiring( 1, 1 ); 
}

/*-------------------------------------------------------------------*/
/*  Mapper 49 Write to Sram Function  映射器 49：写 SRAM 函数        */
/*-------------------------------------------------------------------*/
void Map49_Sram( uint16_t address, uint8_t byte_data )
{
  if ( Map49_Regs[ 2 ] & 0x80 )
  {
    Map49_Regs[ 1 ] = byte_data;
    Map49_Set_CPU_Banks();
    Map49_Set_PPU_Banks();
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 49 Write Function  映射器 49：写函数                      */
/*-------------------------------------------------------------------*/
void Map49_Write( uint16_t address, uint8_t byte_data )
{
  switch ( address & 0xe001 )
  {
    case 0x8000:
      if ( ( byte_data & 0x40 ) != ( Map49_Regs[ 0 ] & 0x40 ) )
      {
        Map49_Set_CPU_Banks();
      }
      if ( ( byte_data & 0x80 ) != ( Map49_Regs[ 0 ] & 0x80 ) )
      {
        Map49_Regs[ 0 ] = byte_data;
        Map49_Set_PPU_Banks();
      }
      Map49_Regs[ 0 ] = byte_data;
      break;

    case 0x8001:
      switch ( Map49_Regs[ 0 ] & 0x07 )
      {
        /* Set PPU Banks  设置 PPU 银行 */
        case 0x00:
          Map49_Chr01 = byte_data & 0xfe;
          Map49_Set_PPU_Banks();
          break;

        case 0x01:
          Map49_Chr23 = byte_data & 0xfe;
          Map49_Set_PPU_Banks();
          break;

        case 0x02:
          Map49_Chr4 = byte_data;
          Map49_Set_PPU_Banks();
          break;

        case 0x03:
          Map49_Chr5 = byte_data;
          Map49_Set_PPU_Banks();
          break;

        case 0x04:
          Map49_Chr6 = byte_data;
          Map49_Set_PPU_Banks();
          break;

        case 0x05:
          Map49_Chr7 = byte_data;
          Map49_Set_PPU_Banks();
          break;

        /* Set ROM Banks  设置 ROM 银行 */
        case 0x06:
          Map49_Prg0 = byte_data;
          Map49_Set_CPU_Banks();
          break;

        case 0x07:
          Map49_Prg1 = byte_data;
          Map49_Set_CPU_Banks();
          break;
      }
      break;

    case 0xa000:
      if ( !ROM_FourScr )
      {
        if ( byte_data & 0x01 )
        {
          InfoNES_Mirroring( 0 );
        } else {
          InfoNES_Mirroring( 1 );
        }
      }
      break;

    case 0xa001:
      Map49_Regs[ 2 ] = byte_data;
      break;

    case 0xc000:
      Map49_IRQ_Cnt = byte_data;
      break;

    case 0xc001:
      Map49_IRQ_Latch = byte_data;
      break;

    case 0xe000:
      Map49_IRQ_Enable = 0;
      break;

    case 0xe001:
      Map49_IRQ_Enable = 1;
      break;
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 49 H-Sync Function  映射器 49：行同步函数                 */
/*-------------------------------------------------------------------*/
void Map49_HSync()
{
/*
 *  Callback at HSync  行同步回调
 *
 */
  if ( Map49_IRQ_Enable )
  {
    if ( 0 <= PPU_Scanline && PPU_Scanline <= 239 )
    {
      if ( PPU_R1 & R1_SHOW_SCR || PPU_R1 & R1_SHOW_SP )
      {
        if ( !( Map49_IRQ_Cnt-- ) )
        {
          Map49_IRQ_Cnt = Map49_IRQ_Latch;
          IRQ_REQ;
        }
      }
    }
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 49 Set CPU Banks Function  映射器 49：设置 CPU 银行函数   */
/*-------------------------------------------------------------------*/
void Map49_Set_CPU_Banks()
{
  uint32_t bank0, bank1, bank2, bank3;

  if ( Map49_Regs[ 1 ] & 0x01 )
  {
    if ( Map49_Regs[ 0 ] & 0x40 )
    {
      bank0 = ( ( ( NesHeader.rom_size << 1 ) - 1 ) & 0x0e ) | ( ( Map49_Regs[ 1 ] & 0xc0 ) >> 2 );
      bank1 = ( Map49_Prg1 & 0x0f ) | ( ( Map49_Regs[ 1 ] & 0xc0 ) >> 2 );
      bank2 = ( Map49_Prg0 & 0x0f ) | ( ( Map49_Regs[ 1 ] & 0xc0 ) >> 2 );
      bank3 = ( ( ( NesHeader.rom_size << 1 ) - 1 ) & 0x0f ) | ( ( Map49_Regs[ 1 ] & 0xc0 ) >> 2 );
    } else {
      bank0 = ( Map49_Prg0 & 0x0f ) | ( ( Map49_Regs[ 1 ] & 0xc0 ) >> 2 );
      bank1 = ( Map49_Prg1 & 0x0f ) | ( ( Map49_Regs[ 1 ] & 0xc0 ) >> 2 );
      bank2 = ( ( ( NesHeader.rom_size << 1 ) - 1 ) & 0x0e ) | ( ( Map49_Regs[ 1 ] & 0xc0 ) >> 2 );
      bank3 = ( ( ( NesHeader.rom_size << 1 ) - 1 ) & 0x0f ) | ( ( Map49_Regs[ 1 ] & 0xc0 ) >> 2 );
    }
  } else {
    bank0 = ( ( Map49_Regs[ 1 ] & 0x70 ) >> 2 ) | 0;
    bank1 = ( ( Map49_Regs[ 1 ] & 0x70 ) >> 2 ) | 1;
    bank2 = ( ( Map49_Regs[ 1 ] & 0x70 ) >> 2 ) | 2;
    bank3 = ( ( Map49_Regs[ 1 ] & 0x70 ) >> 2 ) | 3;
  }

  /* Set ROM Banks  设置 ROM 银行 */
  ROMBANK0 = ROMPAGE( bank0 % ( NesHeader.rom_size << 1) );
  ROMBANK1 = ROMPAGE( bank1 % ( NesHeader.rom_size << 1) );
  ROMBANK2 = ROMPAGE( bank2 % ( NesHeader.rom_size << 1) );
  ROMBANK3 = ROMPAGE( bank3 % ( NesHeader.rom_size << 1) );
}

/*-------------------------------------------------------------------*/
/*  Mapper 49 Set PPU Banks Function  映射器 49：设置 PPU 银行函数   */
/*-------------------------------------------------------------------*/
void Map49_Set_PPU_Banks()
{
  Map49_Chr01 = ( Map49_Chr01 & 0x7f ) | ( ( Map49_Regs[ 1 ] & 0xc0 ) << 1 );
  Map49_Chr23 = ( Map49_Chr23 & 0x7f ) | ( ( Map49_Regs[ 1 ] & 0xc0 ) << 1 );
  Map49_Chr4 = ( Map49_Chr4 & 0x7f ) | ( ( Map49_Regs[ 1 ] & 0xc0 ) << 1 );
  Map49_Chr5 = ( Map49_Chr5 & 0x7f ) | ( ( Map49_Regs[ 1 ] & 0xc0 ) << 1 );
  Map49_Chr6 = ( Map49_Chr6 & 0x7f ) | ( ( Map49_Regs[ 1 ] & 0xc0 ) << 1 );
  Map49_Chr7 = ( Map49_Chr7 & 0x7f ) | ( ( Map49_Regs[ 1 ] & 0xc0 ) << 1 );

  /* Set PPU Banks  设置 PPU 银行 */
  if ( Map49_Regs[ 0 ] & 0x80 )
  { 
    PPUBANK[ 0 ] = VROMPAGE( Map49_Chr4 % ( NesHeader.v_rom_size << 3 ) );
    PPUBANK[ 1 ] = VROMPAGE( Map49_Chr5 % ( NesHeader.v_rom_size << 3 ) );
    PPUBANK[ 2 ] = VROMPAGE( Map49_Chr6 % ( NesHeader.v_rom_size << 3 ) );
    PPUBANK[ 3 ] = VROMPAGE( Map49_Chr7 % ( NesHeader.v_rom_size << 3 ) );
    PPUBANK[ 4 ] = VROMPAGE( ( Map49_Chr01 + 0 ) % ( NesHeader.v_rom_size << 3 ) );
    PPUBANK[ 5 ] = VROMPAGE( ( Map49_Chr01 + 1 ) % ( NesHeader.v_rom_size << 3 ) );
    PPUBANK[ 6 ] = VROMPAGE( ( Map49_Chr23 + 0 ) % ( NesHeader.v_rom_size << 3 ) );
    PPUBANK[ 7 ] = VROMPAGE( ( Map49_Chr23 + 1 ) % ( NesHeader.v_rom_size << 3 ) );
    InfoNES_SetupChr();
  } else {
    PPUBANK[ 0 ] = VROMPAGE( ( Map49_Chr01 + 0 ) % ( NesHeader.v_rom_size << 3 ) );
    PPUBANK[ 1 ] = VROMPAGE( ( Map49_Chr01 + 1 ) % ( NesHeader.v_rom_size << 3 ) );
    PPUBANK[ 2 ] = VROMPAGE( ( Map49_Chr23 + 0 ) % ( NesHeader.v_rom_size << 3 ) );
    PPUBANK[ 3 ] = VROMPAGE( ( Map49_Chr23 + 1 ) % ( NesHeader.v_rom_size << 3 ) );
    PPUBANK[ 4 ] = VROMPAGE( Map49_Chr4 % ( NesHeader.v_rom_size << 3 ) );
    PPUBANK[ 5 ] = VROMPAGE( Map49_Chr5 % ( NesHeader.v_rom_size << 3 ) );
    PPUBANK[ 6 ] = VROMPAGE( Map49_Chr6 % ( NesHeader.v_rom_size << 3 ) );
    PPUBANK[ 7 ] = VROMPAGE( Map49_Chr7 % ( NesHeader.v_rom_size << 3 ) );
    InfoNES_SetupChr();
  }
}
