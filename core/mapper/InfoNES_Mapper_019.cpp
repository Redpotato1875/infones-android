/*===================================================================*/
/*                                                                   */
/*                    Mapper 19 (Namcot 106)  映射器 19（Namcot 106） */
/*                                                                   */
/*===================================================================*/

uint8_t  Map19_Chr_Ram[ 0x2000 ];
uint8_t  Map19_Regs[ 2 ];

uint8_t  Map19_IRQ_Enable;
uint32_t Map19_IRQ_Cnt;

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 19  初始化映射器 19                            */
/*-------------------------------------------------------------------*/
void Map19_Init()
{
  /* Initialize Mapper  初始化映射器 */
  MapperInit = Map19_Init;

  /* Write to Mapper  写入映射器 */
  MapperWrite = Map19_Write;

  /* Write to SRAM  写入 SRAM */
  MapperSram = Map0_Sram;

  /* Write to APU  写入 APU */
  MapperApu = Map19_Apu;

  /* Read from APU  从 APU 读取 */
  MapperReadApu = Map19_ReadApu;

  /* Callback at VSync  场同步回调 */
  MapperVSync = Map0_VSync;

  /* Callback at HSync  行同步回调 */
  MapperHSync = Map19_HSync;

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
    uint32_t last_page = (uint32_t)NesHeader.v_rom_size << 3;
    PPUBANK[ 0 ] = VROMPAGE( last_page - 8 );
    PPUBANK[ 1 ] = VROMPAGE( last_page - 7 );
    PPUBANK[ 2 ] = VROMPAGE( last_page - 6 );
    PPUBANK[ 3 ] = VROMPAGE( last_page - 5 );
    PPUBANK[ 4 ] = VROMPAGE( last_page - 4 );
    PPUBANK[ 5 ] = VROMPAGE( last_page - 3 );
    PPUBANK[ 6 ] = VROMPAGE( last_page - 2 );
    PPUBANK[ 7 ] = VROMPAGE( last_page - 1 );
    InfoNES_SetupChr();
  }

  /* Initialize State Register  初始化状态寄存器 */
  Map19_Regs[ 0 ] = 0x00;
  Map19_Regs[ 1 ] = 0x00;
  Map19_Regs[ 2 ] = 0x00;

  /* Set up wiring of the interrupt pin  设置中断引脚接线方式 */
  K6502_Set_Int_Wiring( 1, 1 ); 
}

/*-------------------------------------------------------------------*/
/*  Mapper 19 Write Function  映射器 19：写函数                      */
/*-------------------------------------------------------------------*/
void Map19_Write( uint16_t address, uint8_t byte_data )
{
  /* Set PPU Banks  设置 PPU 银行 */
  switch ( address & 0xf800 )
  {
    case 0x8000:  /* $8000-87ff */
      if ( byte_data < 0xe0 || Map19_Regs[ 0 ] == 1 )
      {
        byte_data %= ( NesHeader.v_rom_size << 3 );
        PPUBANK[ 0 ] = VROMPAGE( byte_data );
      } else {
        PPUBANK[ 0 ] = Map19_VROMPAGE( 0 );
      }
      InfoNES_SetupChr();
      break;

    case 0x8800:  /* $8800-8fff */
      if ( byte_data < 0xe0 || Map19_Regs[ 0 ] == 1 )
      {
        byte_data %= ( NesHeader.v_rom_size << 3 );
        PPUBANK[ 1 ] = VROMPAGE( byte_data );
      } else {
        PPUBANK[ 1 ] = Map19_VROMPAGE( 1 );
      }
      InfoNES_SetupChr();
      break;

    case 0x9000:  /* $9000-97ff */
      if ( byte_data < 0xe0 || Map19_Regs[ 0 ] == 1 )
      {
        byte_data %= ( NesHeader.v_rom_size << 3 );
        PPUBANK[ 2 ] = VROMPAGE( byte_data );
      } else {
        PPUBANK[ 2 ] = Map19_VROMPAGE( 2 );
      }
      InfoNES_SetupChr();
      break;

    case 0x9800:  /* $9800-9fff */
      if ( byte_data < 0xe0 || Map19_Regs[ 0 ] == 1 )
      {
        byte_data %= ( NesHeader.v_rom_size << 3 );
        PPUBANK[ 3 ] = VROMPAGE( byte_data );
      } else {
        PPUBANK[ 3 ] = Map19_VROMPAGE( 3 );
      }
      InfoNES_SetupChr();
      break;

    case 0xa000:  /* $a000-a7ff */
      if ( byte_data < 0xe0 || Map19_Regs[ 0 ] == 1 )
      {
        byte_data %= ( NesHeader.v_rom_size << 3 );
        PPUBANK[ 4 ] = VROMPAGE( byte_data );
      } else {
        PPUBANK[ 4 ] = Map19_VROMPAGE( 4 );
      }
      InfoNES_SetupChr();
      break;

    case 0xa800:  /* $a800-afff */
      if ( byte_data < 0xe0 || Map19_Regs[ 0 ] == 1 )
      {
        byte_data %= ( NesHeader.v_rom_size << 3 );
        PPUBANK[ 5 ] = VROMPAGE( byte_data );
      } else {
        PPUBANK[ 5 ] = Map19_VROMPAGE( 5 );
      }
      InfoNES_SetupChr();
      break;

    case 0xb000:  /* $b000-b7ff */
      if ( byte_data < 0xe0 || Map19_Regs[ 0 ] == 1 )
      {
        byte_data %= ( NesHeader.v_rom_size << 3 );
        PPUBANK[ 6 ] = VROMPAGE( byte_data );
      } else {
        PPUBANK[ 6 ] = Map19_VROMPAGE( 6 );
      }
      InfoNES_SetupChr();
      break;

    case 0xb800:  /* $b800-bfff */
      if ( byte_data < 0xe0 || Map19_Regs[ 0 ] == 1 )
      {
        byte_data %= ( NesHeader.v_rom_size << 3 );
        PPUBANK[ 7 ] = VROMPAGE( byte_data );
      } else {
        PPUBANK[ 7 ] = Map19_VROMPAGE( 7 );
      }
      InfoNES_SetupChr();
      break;

    case 0xc000:  /* $c000-c7ff */
      if ( byte_data < 0xe0 || Map19_Regs[ 0 ] == 1 )
      {
        byte_data %= ( NesHeader.v_rom_size << 3 );
        PPUBANK[ NAME_TABLE0 ] = VROMPAGE( byte_data );
      } else {
        PPUBANK[ NAME_TABLE0 ] = VRAMPAGE( byte_data & 0x01 );
      }
      break;

    case 0xc800:  /* $c800-cfff */
      if ( byte_data < 0xe0 || Map19_Regs[ 0 ] == 1 )
      {
        byte_data %= ( NesHeader.v_rom_size << 3 );
        PPUBANK[ NAME_TABLE1 ] = VROMPAGE( byte_data );
      } else {
        PPUBANK[ NAME_TABLE1 ] = VRAMPAGE( byte_data & 0x01 );
      }
      break;

    case 0xd000:  /* $d000-d7ff */
      if ( byte_data < 0xe0 || Map19_Regs[ 0 ] == 1 )
      {
        byte_data %= ( NesHeader.v_rom_size << 3 );
        PPUBANK[ NAME_TABLE2 ] = VROMPAGE( byte_data );
      } else {
        PPUBANK[ NAME_TABLE2 ] = VRAMPAGE( byte_data & 0x01 );
      }
      break;

    case 0xd800:  /* $d800-dfff */
      if ( byte_data < 0xe0 || Map19_Regs[ 0 ] == 1 )
      {
        byte_data %= ( NesHeader.v_rom_size << 3 );
        PPUBANK[ NAME_TABLE3 ] = VROMPAGE( byte_data );
      } else {
        PPUBANK[ NAME_TABLE3 ] = VRAMPAGE( byte_data & 0x01 );
      }
      break;

    case 0xe000:  /* $e000-e7ff */
      byte_data &= 0x3f;
      byte_data %= ( NesHeader.rom_size << 1 );
      ROMBANK0 = ROMPAGE( byte_data );
      break;

    case 0xe800:  /* $e800-efff */
      Map19_Regs[ 0 ] = ( byte_data & 0x40 ) >> 6;
      Map19_Regs[ 1 ] = ( byte_data & 0x80 ) >> 7;

      byte_data &= 0x3f;
      byte_data %= ( NesHeader.rom_size << 1 );
      ROMBANK1 = ROMPAGE( byte_data );
      break;

    case 0xf000:  /* $f000-f7ff */
      byte_data &= 0x3f;
      byte_data %= ( NesHeader.rom_size << 1 );
      ROMBANK2 = ROMPAGE( byte_data );
      break;

    case 0xf800:  /* $e800-efff */
      if ( address == 0xf800 )
      {
        // Extra Sound  额外声音
      }
      break;    
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 19 Write to APU Function  映射器 19：写 APU 函数          */
/*-------------------------------------------------------------------*/
void Map19_Apu( uint16_t address, uint8_t byte_data )
{
  switch ( address & 0xf800 )
  {
    case 0x4800:
      if ( address == 0x4800 )
      {
        // Extra Sound  额外声音
      }
      break;

    case 0x5000:  /* $5000-57ff */
      Map19_IRQ_Cnt = ( Map19_IRQ_Cnt & 0xff00 ) | byte_data;
      break;

    case 0x5800:  /* $5800-5fff */
      Map19_IRQ_Cnt = ( Map19_IRQ_Cnt & 0x00ff ) | ( (uint32_t)( byte_data & 0x7f ) << 8 );
      Map19_IRQ_Enable = ( byte_data & 0x80 ) >> 7;
      break;
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 19 Read from APU Function  映射器 19：读 APU 函数         */
/*-------------------------------------------------------------------*/
uint8_t Map19_ReadApu( uint16_t address )
{
  switch ( address & 0xf800 )
  {
    case 0x4800:
      if ( address == 0x4800 )
      {
        // Extra Sound  额外声音
      }
      return (uint8_t)( address >> 8 );

    case 0x5000:  /* $5000-57ff */
      return (uint8_t)(Map19_IRQ_Cnt & 0x00ff );

    case 0x5800:  /* $5800-5fff */
      return (uint8_t)( (Map19_IRQ_Cnt & 0x7f00) >> 8 );

    default:
      return (uint8_t)( address >> 8 );
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 19 H-Sync Function  映射器 19：行同步函数                 */
/*-------------------------------------------------------------------*/
void Map19_HSync()
{
/*
 *  Callback at HSync  行同步回调
 *
 */
  uint8_t Map19_IRQ_Timing = 113;

  if ( Map19_IRQ_Enable )
  {
    if ( Map19_IRQ_Cnt >= (uint32_t)(0x7fff - Map19_IRQ_Timing) )
    {
      Map19_IRQ_Cnt = 0x7fff;
      IRQ_REQ;
    } else {
      Map19_IRQ_Cnt += Map19_IRQ_Timing;
    }
  }
}
