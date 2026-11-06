/*===================================================================*/
/*                                                                   */
/*                       Mapper 114 (PC-SuperGames)  映射器 114（PC-SuperGames） */
/*                                                                   */
/*===================================================================*/

uint8_t  Map114_Regs[ 8 ];
uint32_t Map114_Prg0, Map114_Prg1;
uint32_t Map114_Chr01, Map114_Chr23;
uint32_t Map114_Chr4, Map114_Chr5, Map114_Chr6, Map114_Chr7;

uint8_t Map114_IRQ_Enable;
uint8_t Map114_IRQ_Cnt;
uint8_t Map114_IRQ_Latch;

#define Map114_Chr_Swap()    ( Map114_Regs[ 0 ] & 0x80 )
#define Map114_Prg_Swap()    ( Map114_Regs[ 0 ] & 0x40 )

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 114  初始化映射器 114                          */
/*-------------------------------------------------------------------*/
void Map114_Init()
{
  /* Initialize Mapper  初始化映射器 */
  MapperInit = Map114_Init;

  /* Write to Mapper  写入映射器 */
  MapperWrite = Map114_Write;

  /* Write to SRAM  写入 SRAM */
  MapperSram = Map114_Sram;

  /* Write to APU  写入 APU */
  MapperApu = Map0_Apu;

  /* Read from APU  从 APU 读取 */
  MapperReadApu = Map0_ReadApu;

  /* Callback at VSync  场同步回调 */
  MapperVSync = Map0_VSync;

  /* Callback at HSync  行同步回调 */
  MapperHSync = Map114_HSync;

  /* Callback at PPU  PPU 回调 */
  MapperPPU = Map0_PPU;

  /* Callback at Rendering Screen ( 1:BG, 0:Sprite )  渲染画面回调（1：背景，0：精灵） */
  MapperRenderScreen = Map0_RenderScreen;

  /* Set SRAM Banks  设置 SRAM 银行 */
  SRAMBANK = SRAM;

  /* Initialize State Registers  初始化状态寄存器 */
  for ( int page_number = 0; page_number < 8; page_number++)
  {
    Map114_Regs[ page_number ] = 0x00;
  }

  /* Set ROM Banks  设置 ROM 银行 */
  Map114_Prg0 = 0;
  Map114_Prg1 = 1;
  Map114_Set_CPU_Banks();

  /* Set PPU Banks  设置 PPU 银行 */
  if ( NesHeader.v_rom_size > 0 )
  {
    Map114_Chr01 = 0;
    Map114_Chr23 = 2;
    Map114_Chr4  = 4;
    Map114_Chr5  = 5;
    Map114_Chr6  = 6;
    Map114_Chr7  = 7;
    Map114_Set_PPU_Banks();
  } else {
    Map114_Chr01 = Map114_Chr23 = 0;
    Map114_Chr4 = Map114_Chr5 = Map114_Chr6 = Map114_Chr7 = 0;
  }

  /* Initialize IRQ Registers  初始化 IRQ 寄存器 */
  Map114_IRQ_Enable = 0;
  Map114_IRQ_Cnt = 0;
  Map114_IRQ_Latch = 0;

  /* Set up wiring of the interrupt pin  设置中断引脚接线方式 */
  K6502_Set_Int_Wiring( 1, 1 ); 
}

/*-------------------------------------------------------------------*/
/*  Mapper 114 Write to Sram Function  映射器 114：写 SRAM 函数      */
/*-------------------------------------------------------------------*/
void Map114_Sram( uint16_t address, uint8_t byte_data )
{
  if ( ( address == 0x6000 ) && ( byte_data == 0x00 ) )
  {
    /* Initialize State Registers  初始化状态寄存器 */
    for ( int page_number = 0; page_number < 8; page_number++)
    {
      Map114_Regs[ page_number ] = 0x00;
    }

    /* Set ROM Banks  设置 ROM 银行 */
    Map114_Prg0 = 0;
    Map114_Prg1 = 1;
    Map114_Set_CPU_Banks();

    /* Set PPU Banks  设置 PPU 银行 */
    if ( NesHeader.v_rom_size > 0 )
    { 
      Map114_Chr01 = 0;
      Map114_Chr23 = 2;
      Map114_Chr4  = 4;
      Map114_Chr5  = 5;
      Map114_Chr6  = 6;
      Map114_Chr7  = 7;
      Map114_Set_PPU_Banks();
    } else {
      Map114_Chr01 = Map114_Chr23 = 0;
      Map114_Chr4 = Map114_Chr5 = Map114_Chr6 = Map114_Chr7 = 0;
    }

    /* Initialize IRQ Registers  初始化 IRQ 寄存器 */
    Map114_IRQ_Enable = 0;
    Map114_IRQ_Cnt = 0;
    Map114_IRQ_Latch = 0;
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 114 Write Function  映射器 114：写函数                    */
/*-------------------------------------------------------------------*/
void Map114_Write( uint16_t address, uint8_t byte_data )
{
  uint32_t bank_num;

  switch ( address & 0xe001 )
  {
    case 0x8000:
      Map114_Regs[ 0 ] = byte_data;
      Map114_Set_PPU_Banks();
      Map114_Set_CPU_Banks();
      break;

    case 0x8001:
      Map114_Regs[ 1 ] = byte_data;
      bank_num = Map114_Regs[ 1 ];

      switch ( Map114_Regs[ 0 ] & 0x07 )
      {
        /* Set PPU Banks  设置 PPU 银行 */
        case 0x00:
          if ( NesHeader.v_rom_size > 0 )
          {
            bank_num &= 0xfe;
            Map114_Chr01 = bank_num;
            Map114_Set_PPU_Banks();
          }
          break;

        case 0x01:
          if ( NesHeader.v_rom_size > 0 )
          {
            bank_num &= 0xfe;
            Map114_Chr23 = bank_num;
            Map114_Set_PPU_Banks();
          }
          break;

        case 0x02:
          if ( NesHeader.v_rom_size > 0 )
          {
            Map114_Chr4 = bank_num;
            Map114_Set_PPU_Banks();
          }
          break;

        case 0x03:
          if ( NesHeader.v_rom_size > 0 )
          {
            Map114_Chr5 = bank_num;
            Map114_Set_PPU_Banks();
          }
          break;

        case 0x04:
          if ( NesHeader.v_rom_size > 0 )
          {
            Map114_Chr6 = bank_num;
            Map114_Set_PPU_Banks();
          }
          break;

        case 0x05:
          if ( NesHeader.v_rom_size > 0 )
          {
            Map114_Chr7 = bank_num;
            Map114_Set_PPU_Banks();
          }
          break;

        /* Set ROM Banks  设置 ROM 银行 */
        case 0x06:
          Map114_Prg0 = bank_num;
          Map114_Set_CPU_Banks();
          break;

        case 0x07:
          Map114_Prg1 = bank_num;
          Map114_Set_CPU_Banks();
          break;
      }
      break;

    case 0xa000:
      Map114_Regs[ 2 ] = byte_data;

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
      Map114_Regs[ 3 ] = byte_data;

      if ( byte_data & 0x80 )
      {
        /* Enable Save RAM $6000-$7fff  启用 $6000-$7fff 的存档 RAM */
      } else {
        /* Disable Save RAM $6000-$7fff  禁用 $6000-$7fff 的存档 RAM */
      }
      break;

    case 0xc000:
      Map114_Regs[ 4 ] = byte_data;
      Map114_IRQ_Cnt = Map114_Regs[ 4 ];
      break;

    case 0xc001:
      Map114_Regs[ 5 ] = byte_data;
      Map114_IRQ_Latch = Map114_Regs[ 5 ];
      break;

    case 0xe000:
      Map114_Regs[ 6 ] = byte_data;
      Map114_IRQ_Enable = 0;
      break;

    case 0xe001:
      Map114_Regs[ 7 ] = byte_data;
      Map114_IRQ_Enable = 1;
      break;
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 114 H-Sync Function  映射器 114：行同步函数               */
/*-------------------------------------------------------------------*/
void Map114_HSync()
{
/*
 *  Callback at HSync  行同步回调
 *
 */
  if ( Map114_IRQ_Enable )
  {
    if ( 0 <= PPU_Scanline && PPU_Scanline <= 239 )
    {
      if ( PPU_R1 & R1_SHOW_SCR || PPU_R1 & R1_SHOW_SP )
      {
        if ( !( Map114_IRQ_Cnt-- ) )
        {
          Map114_IRQ_Cnt = Map114_IRQ_Latch;
          IRQ_REQ;
        }
      }
    }
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 114 Set CPU Banks Function  映射器 114：设置 CPU 银行函数 */
/*-------------------------------------------------------------------*/
void Map114_Set_CPU_Banks()
{
  if ( Map114_Prg_Swap() )
  {
    ROMBANK0 = ROMLASTPAGE( 1 );
    ROMBANK1 = ROMPAGE( Map114_Prg1 % ( NesHeader.rom_size << 1 ) );
    ROMBANK2 = ROMPAGE( Map114_Prg0 % ( NesHeader.rom_size << 1 ) );
    ROMBANK3 = ROMLASTPAGE( 0 );
  } else {
    ROMBANK0 = ROMPAGE( Map114_Prg0 % ( NesHeader.rom_size << 1 ) );
    ROMBANK1 = ROMPAGE( Map114_Prg1 % ( NesHeader.rom_size << 1 ) );
    ROMBANK2 = ROMLASTPAGE( 1 );
    ROMBANK3 = ROMLASTPAGE( 0 );
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 114 Set PPU Banks Function  映射器 114：设置 PPU 银行函数 */
/*-------------------------------------------------------------------*/
void Map114_Set_PPU_Banks()
{
  if ( NesHeader.v_rom_size > 0 )
  {
    if ( Map114_Chr_Swap() )
    { 
      PPUBANK[ 0 ] = VROMPAGE( Map114_Chr4 % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 1 ] = VROMPAGE( Map114_Chr5 % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 2 ] = VROMPAGE( Map114_Chr6 % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 3 ] = VROMPAGE( Map114_Chr7 % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 4 ] = VROMPAGE( ( Map114_Chr01 + 0 ) % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 5 ] = VROMPAGE( ( Map114_Chr01 + 1 ) % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 6 ] = VROMPAGE( ( Map114_Chr23 + 0 ) % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 7 ] = VROMPAGE( ( Map114_Chr23 + 1 ) % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();
    } else {
      PPUBANK[ 0 ] = VROMPAGE( ( Map114_Chr01 + 0 ) % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 1 ] = VROMPAGE( ( Map114_Chr01 + 1 ) % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 2 ] = VROMPAGE( ( Map114_Chr23 + 0 ) % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 3 ] = VROMPAGE( ( Map114_Chr23 + 1 ) % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 4 ] = VROMPAGE( Map114_Chr4 % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 5 ] = VROMPAGE( Map114_Chr5 % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 6 ] = VROMPAGE( Map114_Chr6 % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 7 ] = VROMPAGE( Map114_Chr7 % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();
    }
  }
}
