/*===================================================================*/
/*                                                                   */
/*                        Mapper 5 (MMC5)  映射器 5（MMC5）          */
/*                                                                   */
/*===================================================================*/

uint8_t Map5_Wram[ 0x2000 * 8 ];
uint8_t Map5_Ex_Ram[ 0x400 ]; 
uint8_t Map5_Ex_Vram[ 0x400 ];
uint8_t Map5_Ex_Nam[ 0x400 ];

uint8_t Map5_Prg_Reg[ 8 ];
uint8_t Map5_Wram_Reg[ 8 ];
uint8_t Map5_Chr_Reg[ 8 ][ 2 ];

uint8_t Map5_IRQ_Enable;
uint8_t Map5_IRQ_Status;
uint8_t Map5_IRQ_Line;

uint32_t Map5_Value0;
uint32_t Map5_Value1;

uint8_t Map5_Wram_Protect0;
uint8_t Map5_Wram_Protect1;
uint8_t Map5_Prg_Size;
uint8_t Map5_Chr_Size;
uint8_t Map5_Gfx_Mode;

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 5  初始化映射器 5                              */
/*-------------------------------------------------------------------*/
void Map5_Init()
{
  int page_number;

  /* Initialize Mapper  初始化映射器 */
  MapperInit = Map5_Init;

  /* Write to Mapper  写入映射器 */
  MapperWrite = Map5_Write;

  /* Write to SRAM  写入 SRAM */
  MapperSram = Map0_Sram;

  /* Write to APU  写入 APU */
  MapperApu = Map5_Apu;

  /* Read from APU  从 APU 读取 */
  MapperReadApu = Map5_ReadApu;

  /* Callback at VSync  场同步回调 */
  MapperVSync = Map0_VSync;

  /* Callback at HSync  行同步回调 */
  MapperHSync = Map5_HSync;

  /* Callback at PPU  PPU 回调 */
  MapperPPU = Map0_PPU;

  /* Callback at Rendering Screen ( 1:BG, 0:Sprite )  渲染画面回调（1：背景，0：精灵） */
  MapperRenderScreen = Map5_RenderScreen;

  /* Set SRAM Banks  设置 SRAM 银行 */
  SRAMBANK = SRAM;

  /* Set ROM Banks  设置 ROM 银行 */
  ROMBANK0 = ROMLASTPAGE( 0 );
  ROMBANK1 = ROMLASTPAGE( 0 );
  ROMBANK2 = ROMLASTPAGE( 0 );
  ROMBANK3 = ROMLASTPAGE( 0 );

  /* Set PPU Banks  设置 PPU 银行 */
  for ( page_number = 0; page_number < 8; ++page_number )
    PPUBANK[ page_number ] = VROMPAGE( page_number );
  InfoNES_SetupChr();

  /* Initialize State Registers  初始化状态寄存器 */
  for ( page_number = 4; page_number < 8; ++page_number )
  {
    Map5_Prg_Reg[ page_number ] = ( NesHeader.rom_size << 1 ) - 1;
    Map5_Wram_Reg[ page_number ] = 0xff;
  }
  Map5_Wram_Reg[ 3 ] = 0xff;

  for ( uint8_t page_number = 4; page_number < 8; ++page_number )
  {
    Map5_Chr_Reg[ page_number ][ 0 ] = page_number;
    Map5_Chr_Reg[ page_number ][ 1 ] = ( page_number & 0x03 ) + 4;
  }

  InfoNES_MemorySet( Map5_Wram, 0x00, sizeof( Map5_Wram ) );
  InfoNES_MemorySet( Map5_Ex_Ram, 0x00, sizeof( Map5_Ex_Ram ) );
  InfoNES_MemorySet( Map5_Ex_Vram, 0x00, sizeof( Map5_Ex_Vram ) );
  InfoNES_MemorySet( Map5_Ex_Nam, 0x00, sizeof( Map5_Ex_Nam ) );

  Map5_Prg_Size = 3;
  Map5_Wram_Protect0 = 0;
  Map5_Wram_Protect1 = 0;
  Map5_Chr_Size = 3;
  Map5_Gfx_Mode = 0;

  Map5_IRQ_Enable = 0;
  Map5_IRQ_Status = 0;
  Map5_IRQ_Line = 0;

  /* Set up wiring of the interrupt pin  设置中断引脚接线方式 */
  K6502_Set_Int_Wiring( 1, 1 ); 
}

/*-------------------------------------------------------------------*/
/*  Mapper 5 Read from APU Function  映射器 5：读 APU 函数           */
/*-------------------------------------------------------------------*/
uint8_t Map5_ReadApu( uint16_t address )
{
  uint8_t return_value = (uint8_t)( address >> 8 );

  switch ( address )
  {
    case 0x5204:
      return_value = Map5_IRQ_Status;
      Map5_IRQ_Status = 0;
      break;

    case 0x5205:
      return_value = (uint8_t)( ( Map5_Value0 * Map5_Value1 ) & 0x00ff );
      break;

    case 0x5206:
      return_value = (uint8_t)( ( ( Map5_Value0 * Map5_Value1 ) & 0xff00 ) >> 8 );
      break;

    default:
      if ( 0x5c00 <= address && address <= 0x5fff )
      {
        return_value = Map5_Ex_Ram[ address - 0x5c00 ];
      }
      break;
  }
  return return_value;
}

/*-------------------------------------------------------------------*/
/*  Mapper 5 Write to APU Function  映射器 5：写 APU 函数            */
/*-------------------------------------------------------------------*/
void Map5_Apu( uint16_t address, uint8_t byte_data )
{
  int page_number;

  switch ( address )
  {
    case 0x5100:
      Map5_Prg_Size = byte_data & 0x03;
      break;

    case 0x5101:
      Map5_Chr_Size = byte_data & 0x03;
      break;

    case 0x5102:
      Map5_Wram_Protect0 = byte_data & 0x03;
      break;

    case 0x5103:
      Map5_Wram_Protect1 = byte_data & 0x03;
      break;

    case 0x5104:
      Map5_Gfx_Mode = byte_data & 0x03;
      break;

    case 0x5105:
      for ( page_number = 0; page_number < 4; page_number++ )
      {
        uint8_t nam_reg;
        
        nam_reg = byte_data & 0x03;
        byte_data = byte_data >> 2;

        switch ( nam_reg )
        {
          case 0:
#if 1
            PPUBANK[ page_number + 8 ] = VRAMPAGE( 0 );
#else
            PPUBANK[ page_number + 8 ] = CRAMPAGE( 8 );
#endif
            break;
          case 1:
#if 1
            PPUBANK[ page_number + 8 ] = VRAMPAGE( 1 );
#else
            PPUBANK[ page_number + 8 ] = CRAMPAGE( 9 );
#endif
            break;
          case 2:
            PPUBANK[ page_number + 8 ] = Map5_Ex_Vram;
            break;
          case 3:
            PPUBANK[ page_number + 8 ] = Map5_Ex_Nam;
            break;
        }
      }
      break;

    case 0x5106:
      InfoNES_MemorySet( Map5_Ex_Nam, byte_data, 0x3c0 );
      break;

    case 0x5107:
      byte_data &= 0x03;
      byte_data = byte_data | ( byte_data << 2 ) | ( byte_data << 4 ) | ( byte_data << 6 );
      InfoNES_MemorySet( &( Map5_Ex_Nam[ 0x3c0 ] ), byte_data, 0x400 - 0x3c0 );
      break;

    case 0x5113:
      Map5_Wram_Reg[ 3 ] = byte_data & 0x07;
      SRAMBANK = Map5_ROMPAGE( byte_data & 0x07 );
      break;

    case 0x5114:
    case 0x5115:
    case 0x5116:
    case 0x5117:
      Map5_Prg_Reg[ address & 0x07 ] = byte_data;
      Map5_Sync_Prg_Banks();
      break;

    case 0x5120:
    case 0x5121:
    case 0x5122:
    case 0x5123:
    case 0x5124:
    case 0x5125:
    case 0x5126:
    case 0x5127:
      Map5_Chr_Reg[ address & 0x07 ][ 0 ] = byte_data;
      Map5_Sync_Prg_Banks();
      break;

    case 0x5128:
    case 0x5129:
    case 0x512a:
    case 0x512b:
      Map5_Chr_Reg[ ( address & 0x03 ) + 0 ][ 1 ] = byte_data;
      Map5_Chr_Reg[ ( address & 0x03 ) + 4 ][ 1 ] = byte_data;
      break;

    case 0x5200:
    case 0x5201:
    case 0x5202:
      /* Nothing to do  无需处理 */
      break;

    case 0x5203:
      if ( Map5_IRQ_Line >= 0x40 )
      {
        Map5_IRQ_Line = byte_data;
      } else {
        Map5_IRQ_Line += byte_data;
      }
      break;

    case 0x5204:
      Map5_IRQ_Enable = byte_data;
      break;

    case 0x5205:
      Map5_Value0 = byte_data;
      break;

    case 0x5206:
      Map5_Value1 = byte_data;
      break;

    default:
      if ( 0x5000 <= address && address <= 0x5015 )
      {
        /* Extra Sound  额外声音 */
      } else 
      if ( 0x5c00 <= address && address <= 0x5fff )
      {
        switch ( Map5_Gfx_Mode )
        {
          case 0:
            Map5_Ex_Vram[ address - 0x5c00 ] = byte_data;
            break;
          case 2:
            Map5_Ex_Ram[ address - 0x5c00 ] = byte_data;
            break;
        }
      }
      break;
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 5 Write to SRAM Function  映射器 5：写 SRAM 函数          */
/*-------------------------------------------------------------------*/
void Map5_Sram( uint16_t address, uint8_t byte_data )
{
  if ( Map5_Wram_Protect0 == 0x02 && Map5_Wram_Protect1 == 0x01 )
  {
    if ( Map5_Wram_Reg[ 3 ] != 0xff )
    {
      Map5_Wram[ 0x2000 * Map5_Wram_Reg[ 3 ] + ( address - 0x6000) ] = byte_data;
    }
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 5 Write Function  映射器 5：写函数                        */
/*-------------------------------------------------------------------*/
void Map5_Write( uint16_t address, uint8_t byte_data )
{
  if ( Map5_Wram_Protect0 == 0x02 && Map5_Wram_Protect1 == 0x01 )
  {
    switch ( address & 0xe000 )
    {
      case 0x8000:      /* $8000-$9fff */
        if ( Map5_Wram_Reg[ 4 ] != 0xff )
        {
          Map5_Wram[ 0x2000 * Map5_Wram_Reg[ 4 ] + ( address - 0x8000) ] = byte_data;
        }
        break;

      case 0xa000:      /* $a000-$bfff */
        if ( Map5_Wram_Reg[ 5 ] != 0xff )
        {
          Map5_Wram[ 0x2000 * Map5_Wram_Reg[ 5 ] + ( address - 0xa000) ] = byte_data;
        }
        break;

      case 0xc000:      /* $c000-$dfff */
        if ( Map5_Wram_Reg[ 6 ] != 0xff )
        {
          Map5_Wram[ 0x2000 * Map5_Wram_Reg[ 6 ] + ( address - 0xc000) ] = byte_data;
        }
        break;
    }
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 5 H-Sync Function  映射器 5：行同步函数                   */
/*-------------------------------------------------------------------*/
void Map5_HSync()
{
  if ( PPU_Scanline <= 240 )
  {
    if ( PPU_Scanline == Map5_IRQ_Line )
    {
      Map5_IRQ_Status |= 0x80;

      if ( Map5_IRQ_Enable && Map5_IRQ_Line < 0xf0 )
      {
        IRQ_REQ;
      }
      if ( Map5_IRQ_Line >= 0x40 )
      {
        Map5_IRQ_Enable = 0;
      }
    }
  } else {
    Map5_IRQ_Status |= 0x40;
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 5 Rendering Screen Function  映射器 5：渲染画面函数       */
/*-------------------------------------------------------------------*/
void Map5_RenderScreen( uint8_t op_mode )
{
  uint32_t page_count[ 8 ];

  switch ( Map5_Chr_Size )
  {
    case 0:
      page_count[ 7 ] = ( (uint32_t)Map5_Chr_Reg[7][op_mode] << 3 ) % ( NesHeader.v_rom_size << 3 );

      PPUBANK[ 0 ] = VROMPAGE( page_count[ 7 ] + 0 );
      PPUBANK[ 1 ] = VROMPAGE( page_count[ 7 ] + 1 );
      PPUBANK[ 2 ] = VROMPAGE( page_count[ 7 ] + 2 );
      PPUBANK[ 3 ] = VROMPAGE( page_count[ 7 ] + 3 );
      PPUBANK[ 4 ] = VROMPAGE( page_count[ 7 ] + 4 );
      PPUBANK[ 5 ] = VROMPAGE( page_count[ 7 ] + 5 );
      PPUBANK[ 6 ] = VROMPAGE( page_count[ 7 ] + 6 );
      PPUBANK[ 7 ] = VROMPAGE( page_count[ 7 ] + 7 );
      InfoNES_SetupChr();
      break;

    case 1:
      page_count[ 3 ] = ( (uint32_t)Map5_Chr_Reg[3][op_mode] << 2 ) % ( NesHeader.v_rom_size << 3 );
      page_count[ 7 ] = ( (uint32_t)Map5_Chr_Reg[7][op_mode] << 2 ) % ( NesHeader.v_rom_size << 3 );

      PPUBANK[ 0 ] = VROMPAGE( page_count[ 3 ] + 0 );
      PPUBANK[ 1 ] = VROMPAGE( page_count[ 3 ] + 1 );
      PPUBANK[ 2 ] = VROMPAGE( page_count[ 3 ] + 2 );
      PPUBANK[ 3 ] = VROMPAGE( page_count[ 3 ] + 3 );
      PPUBANK[ 4 ] = VROMPAGE( page_count[ 7 ] + 0 );
      PPUBANK[ 5 ] = VROMPAGE( page_count[ 7 ] + 1 );
      PPUBANK[ 6 ] = VROMPAGE( page_count[ 7 ] + 2 );
      PPUBANK[ 7 ] = VROMPAGE( page_count[ 7 ] + 3 );
     InfoNES_SetupChr();
      break;

    case 2:
      page_count[ 1 ] = ( (uint32_t)Map5_Chr_Reg[1][op_mode] << 1 ) % ( NesHeader.v_rom_size << 3 );
      page_count[ 3 ] = ( (uint32_t)Map5_Chr_Reg[3][op_mode] << 1 ) % ( NesHeader.v_rom_size << 3 );
      page_count[ 5 ] = ( (uint32_t)Map5_Chr_Reg[5][op_mode] << 1 ) % ( NesHeader.v_rom_size << 3 );
      page_count[ 7 ] = ( (uint32_t)Map5_Chr_Reg[7][op_mode] << 1 ) % ( NesHeader.v_rom_size << 3 );

      PPUBANK[ 0 ] = VROMPAGE( page_count[ 1 ] + 0 );
      PPUBANK[ 1 ] = VROMPAGE( page_count[ 1 ] + 1 );
      PPUBANK[ 2 ] = VROMPAGE( page_count[ 3 ] + 0 );
      PPUBANK[ 3 ] = VROMPAGE( page_count[ 3 ] + 1 );
      PPUBANK[ 4 ] = VROMPAGE( page_count[ 5 ] + 0 );
      PPUBANK[ 5 ] = VROMPAGE( page_count[ 5 ] + 1 );
      PPUBANK[ 6 ] = VROMPAGE( page_count[ 7 ] + 0 );
      PPUBANK[ 7 ] = VROMPAGE( page_count[ 7 ] + 1 );
      InfoNES_SetupChr();
      break;

    default:
      page_count[ 0 ] = (uint32_t)Map5_Chr_Reg[0][op_mode] % ( NesHeader.v_rom_size << 3 );
      page_count[ 1 ] = (uint32_t)Map5_Chr_Reg[1][op_mode] % ( NesHeader.v_rom_size << 3 );
      page_count[ 2 ] = (uint32_t)Map5_Chr_Reg[2][op_mode] % ( NesHeader.v_rom_size << 3 );
      page_count[ 3 ] = (uint32_t)Map5_Chr_Reg[3][op_mode] % ( NesHeader.v_rom_size << 3 );
      page_count[ 4 ] = (uint32_t)Map5_Chr_Reg[4][op_mode] % ( NesHeader.v_rom_size << 3 );
      page_count[ 5 ] = (uint32_t)Map5_Chr_Reg[5][op_mode] % ( NesHeader.v_rom_size << 3 );
      page_count[ 6 ] = (uint32_t)Map5_Chr_Reg[6][op_mode] % ( NesHeader.v_rom_size << 3 );
      page_count[ 7 ] = (uint32_t)Map5_Chr_Reg[7][op_mode] % ( NesHeader.v_rom_size << 3 );

      PPUBANK[ 0 ] = VROMPAGE( page_count[ 0 ] );
      PPUBANK[ 1 ] = VROMPAGE( page_count[ 1 ] );
      PPUBANK[ 2 ] = VROMPAGE( page_count[ 2 ] );
      PPUBANK[ 3 ] = VROMPAGE( page_count[ 3 ] );
      PPUBANK[ 4 ] = VROMPAGE( page_count[ 4 ] );
      PPUBANK[ 5 ] = VROMPAGE( page_count[ 5 ] );
      PPUBANK[ 6 ] = VROMPAGE( page_count[ 6 ] );
      PPUBANK[ 7 ] = VROMPAGE( page_count[ 7 ] );
      InfoNES_SetupChr();
      break;
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 5 Sync Program Banks Function  映射器 5：同步程序银行函数 */
/*-------------------------------------------------------------------*/
void Map5_Sync_Prg_Banks( void )
{
  switch( Map5_Prg_Size )
  {
    case 0:
      Map5_Wram_Reg[ 4 ] = 0xff;
      Map5_Wram_Reg[ 5 ] = 0xff;
      Map5_Wram_Reg[ 6 ] = 0xff;

      ROMBANK0 = ROMPAGE( ( (Map5_Prg_Reg[7] & 0x7c) + 0 ) % ( NesHeader.rom_size << 1 ) );
      ROMBANK1 = ROMPAGE( ( (Map5_Prg_Reg[7] & 0x7c) + 1 ) % ( NesHeader.rom_size << 1 ) );
      ROMBANK2 = ROMPAGE( ( (Map5_Prg_Reg[7] & 0x7c) + 2 ) % ( NesHeader.rom_size << 1 ) );
      ROMBANK3 = ROMPAGE( ( (Map5_Prg_Reg[7] & 0x7c) + 3 ) % ( NesHeader.rom_size << 1 ) );
      break;

    case 1:
      if ( Map5_Prg_Reg[ 5 ] & 0x80 )
      {
        Map5_Wram_Reg[ 4 ] = 0xff;
        Map5_Wram_Reg[ 5 ] = 0xff;
        ROMBANK0 = ROMPAGE( ( (Map5_Prg_Reg[7] & 0x7e) + 0 ) % ( NesHeader.rom_size << 1 ) );
        ROMBANK1 = ROMPAGE( ( (Map5_Prg_Reg[7] & 0x7e) + 1 ) % ( NesHeader.rom_size << 1 ) );
      } else {
        Map5_Wram_Reg[ 4 ] = ( Map5_Prg_Reg[ 5 ] & 0x06 ) + 0;
        Map5_Wram_Reg[ 5 ] = ( Map5_Prg_Reg[ 5 ] & 0x06 ) + 1;
        ROMBANK0 = Map5_ROMPAGE( Map5_Wram_Reg[ 4 ] );
        ROMBANK1 = Map5_ROMPAGE( Map5_Wram_Reg[ 5 ] );
      }

      Map5_Wram_Reg[ 6 ] = 0xff;
      ROMBANK2 = ROMPAGE( ( (Map5_Prg_Reg[7] & 0x7e) + 0 ) % ( NesHeader.rom_size << 1 ) );
      ROMBANK3 = ROMPAGE( ( (Map5_Prg_Reg[7] & 0x7e) + 1 ) % ( NesHeader.rom_size << 1 ) );
      break;

    case 2:
      if ( Map5_Prg_Reg[ 5 ] & 0x80 )
      {
        Map5_Wram_Reg[ 4 ] = 0xff;
        Map5_Wram_Reg[ 5 ] = 0xff;
        ROMBANK0 = ROMPAGE( ( (Map5_Prg_Reg[5] & 0x7e) + 0 ) % ( NesHeader.rom_size << 1 ) );
        ROMBANK1 = ROMPAGE( ( (Map5_Prg_Reg[5] & 0x7e) + 1 ) % ( NesHeader.rom_size << 1 ) );
      } else {
        Map5_Wram_Reg[ 4 ] = ( Map5_Prg_Reg[ 5 ] & 0x06 ) + 0;
        Map5_Wram_Reg[ 5 ] = ( Map5_Prg_Reg[ 5 ] & 0x06 ) + 1;
        ROMBANK0 = Map5_ROMPAGE( Map5_Wram_Reg[ 4 ] );
        ROMBANK1 = Map5_ROMPAGE( Map5_Wram_Reg[ 5 ] );
      }

      if ( Map5_Prg_Reg[ 6 ] & 0x80 )
      {
        Map5_Wram_Reg[ 6 ] = 0xff;
        ROMBANK2 = ROMPAGE( (Map5_Prg_Reg[6] & 0x7f) % ( NesHeader.rom_size << 1 ) );
      } else {
        Map5_Wram_Reg[ 6 ] = Map5_Prg_Reg[ 6 ] & 0x07;
        ROMBANK2 = Map5_ROMPAGE( Map5_Wram_Reg[ 6 ] );
      }

      ROMBANK3 = ROMPAGE( (Map5_Prg_Reg[7] & 0x7f) % ( NesHeader.rom_size << 1 ) );
      break;

    default:
      if ( Map5_Prg_Reg[ 4 ] & 0x80 )
      {
        Map5_Wram_Reg[ 4 ] = 0xff;
        ROMBANK0 = ROMPAGE( (Map5_Prg_Reg[4] & 0x7f) % ( NesHeader.rom_size << 1 ) );
      } else {
        Map5_Wram_Reg[ 4 ] = Map5_Prg_Reg[ 4 ] & 0x07;
        ROMBANK0 = Map5_ROMPAGE( Map5_Wram_Reg[ 4 ] );
      }

      if ( Map5_Prg_Reg[ 5 ] & 0x80 )
      {
        Map5_Wram_Reg[ 5 ] = 0xff;
        ROMBANK1 = ROMPAGE( (Map5_Prg_Reg[5] & 0x7f) % ( NesHeader.rom_size << 1 ) );
      } else {
        Map5_Wram_Reg[ 5 ] = Map5_Prg_Reg[ 5 ] & 0x07;
        ROMBANK1 = Map5_ROMPAGE( Map5_Wram_Reg[ 5 ] );
      }

      if ( Map5_Prg_Reg[ 6 ] & 0x80 )
      {
        Map5_Wram_Reg[ 6 ] = 0xff;
        ROMBANK2 = ROMPAGE( (Map5_Prg_Reg[6] & 0x7f) % ( NesHeader.rom_size << 1 ) );
      } else {
        Map5_Wram_Reg[ 6 ] = Map5_Prg_Reg[ 6 ] & 0x07;
        ROMBANK2 = Map5_ROMPAGE( Map5_Wram_Reg[ 6 ] );
      }

      ROMBANK3 = ROMPAGE( (Map5_Prg_Reg[7] & 0x7f) % ( NesHeader.rom_size << 1 ) );
      break;
  }
}
