/*===================================================================*/
/*                                                                   */
/*                       Mapper 44 (Nin1)  映射器 44（Nin1）         */
/*                                                                   */
/*===================================================================*/

uint8_t  Map44_Regs[ 8 ];
uint32_t Map44_Rom_Bank;
uint32_t Map44_Prg0, Map44_Prg1;
uint32_t Map44_Chr01, Map44_Chr23;
uint32_t Map44_Chr4, Map44_Chr5, Map44_Chr6, Map44_Chr7;

#define Map44_Chr_Swap()    ( Map44_Regs[ 0 ] & 0x80 )
#define Map44_Prg_Swap()    ( Map44_Regs[ 0 ] & 0x40 )

uint8_t Map44_IRQ_Enable;
uint8_t Map44_IRQ_Cnt;
uint8_t Map44_IRQ_Latch;

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 44  初始化映射器 44                            */
/*-------------------------------------------------------------------*/
void Map44_Init()
{
  /* Initialize Mapper  初始化映射器 */
  MapperInit = Map44_Init;

  /* Write to Mapper  写入映射器 */
  MapperWrite = Map44_Write;

  /* Write to SRAM  写入 SRAM */
  MapperSram = Map0_Sram;

  /* Write to APU  写入 APU */
  MapperApu = Map0_Apu;

  /* Read from APU  从 APU 读取 */
  MapperReadApu = Map0_ReadApu;

  /* Callback at VSync  场同步回调 */
  MapperVSync = Map0_VSync;

  /* Callback at HSync  行同步回调 */
  MapperHSync = Map44_HSync;

  /* Callback at PPU  PPU 回调 */
  MapperPPU = Map0_PPU;

  /* Callback at Rendering Screen ( 1:BG, 0:Sprite )  渲染画面回调（1：背景，0：精灵） */
  MapperRenderScreen = Map0_RenderScreen;

  /* Set SRAM Banks  设置 SRAM 银行 */
  SRAMBANK = SRAM;

  /* Initialize State Registers  初始化状态寄存器 */
  for ( int page_number = 0; page_number < 8; page_number++ )
  {
    Map44_Regs[ page_number ] = 0x00;
  }

  /* Set ROM Banks  设置 ROM 银行 */
  Map44_Rom_Bank = 0;
  Map44_Prg0 = 0;
  Map44_Prg1 = 1;
  Map44_Set_CPU_Banks();

  /* Set PPU Banks  设置 PPU 银行 */
  if ( NesHeader.v_rom_size > 0 )
  {
    Map44_Chr01 = 0;
    Map44_Chr23 = 2;
    Map44_Chr4  = 4;
    Map44_Chr5  = 5;
    Map44_Chr6  = 6;
    Map44_Chr7  = 7;
    Map44_Set_PPU_Banks();
  } else {
    Map44_Chr01 = Map44_Chr23 = 0;
    Map44_Chr4 = Map44_Chr5 = Map44_Chr6 = Map44_Chr7 = 0;
  }

  /* Initialize IRQ Registers  初始化 IRQ 寄存器 */
  Map44_IRQ_Enable = 0;
  Map44_IRQ_Cnt = 0;
  Map44_IRQ_Latch = 0;

  /* Set up wiring of the interrupt pin  设置中断引脚接线方式 */
  K6502_Set_Int_Wiring( 1, 1 ); 
}

/*-------------------------------------------------------------------*/
/*  Mapper 44 Write Function  映射器 44：写函数                      */
/*-------------------------------------------------------------------*/
void Map44_Write( uint16_t address, uint8_t byte_data )
{
  uint32_t bank_num;

  switch ( address & 0xe001 )
  {
    case 0x8000:
      Map44_Regs[ 0 ] = byte_data;
      Map44_Set_PPU_Banks();
      Map44_Set_CPU_Banks();
      break;

    case 0x8001:
      Map44_Regs[ 1 ] = byte_data;
      bank_num = Map44_Regs[ 1 ];

      switch ( Map44_Regs[ 0 ] & 0x07 )
      {
        /* Set PPU Banks  设置 PPU 银行 */
        case 0x00:
          if ( NesHeader.v_rom_size > 0 )
          {
            bank_num &= 0xfe;
            Map44_Chr01 = bank_num;
            Map44_Set_PPU_Banks();
          }
          break;

        case 0x01:
          if ( NesHeader.v_rom_size > 0 )
          {
            bank_num &= 0xfe;
            Map44_Chr23 = bank_num;
            Map44_Set_PPU_Banks();
          }
          break;

        case 0x02:
          if ( NesHeader.v_rom_size > 0 )
          {
            Map44_Chr4 = bank_num;
            Map44_Set_PPU_Banks();
          }
          break;

        case 0x03:
          if ( NesHeader.v_rom_size > 0 )
          {
            Map44_Chr5 = bank_num;
            Map44_Set_PPU_Banks();
          }
          break;

        case 0x04:
          if ( NesHeader.v_rom_size > 0 )
          {
            Map44_Chr6 = bank_num;
            Map44_Set_PPU_Banks();
          }
          break;

        case 0x05:
          if ( NesHeader.v_rom_size > 0 )
          {
            Map44_Chr7 = bank_num;
            Map44_Set_PPU_Banks();
          }
          break;

        /* Set ROM Banks  设置 ROM 银行 */
        case 0x06:
          Map44_Prg0 = bank_num;
          Map44_Set_CPU_Banks();
          break;

        case 0x07:
          Map44_Prg1 = bank_num;
          Map44_Set_CPU_Banks();
          break;
      }
      break;

    case 0xa000:
      Map44_Regs[ 2 ] = byte_data;

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
      Map44_Rom_Bank = byte_data & 0x07;
      if ( Map44_Rom_Bank == 7 )
      {
        Map44_Rom_Bank = 6;
      }
      Map44_Set_CPU_Banks();
      Map44_Set_PPU_Banks();
      break;

    case 0xc000:
      Map44_Regs[ 4 ] = byte_data;
      Map44_IRQ_Cnt = Map44_Regs[ 4 ];
      break;

    case 0xc001:
      Map44_Regs[ 5 ] = byte_data;
      Map44_IRQ_Latch = Map44_Regs[ 5 ];
      break;

    case 0xe000:
      Map44_Regs[ 6 ] = byte_data;
      Map44_IRQ_Enable = 0;
      break;

    case 0xe001:
      Map44_Regs[ 7 ] = byte_data;
      Map44_IRQ_Enable = 1;
      break;
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 44 H-Sync Function  映射器 44：行同步函数                 */
/*-------------------------------------------------------------------*/
void Map44_HSync()
{
/*
 *  Callback at HSync  行同步回调
 *
 */
  if ( Map44_IRQ_Enable )
  {
    if ( 0 <= PPU_Scanline && PPU_Scanline <= 239 )
    {
      if ( PPU_R1 & R1_SHOW_SCR || PPU_R1 & R1_SHOW_SP )
      {
        if ( !( Map44_IRQ_Cnt-- ) )
        {
          Map44_IRQ_Cnt = Map44_IRQ_Latch;
          IRQ_REQ;
        }
      }
    }
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 44 Set CPU Banks Function  映射器 44：设置 CPU 银行函数   */
/*-------------------------------------------------------------------*/
void Map44_Set_CPU_Banks()
{
  if ( Map44_Prg_Swap() )
  {
    ROMBANK0 = ROMPAGE( ( ( Map44_Rom_Bank << 4 ) + 14 ) % ( NesHeader.rom_size << 1 ) );
    ROMBANK1 = ROMPAGE( ( ( Map44_Rom_Bank << 4 ) + Map44_Prg1 ) % ( NesHeader.rom_size << 1 ) );
    ROMBANK2 = ROMPAGE( ( ( Map44_Rom_Bank << 4 ) + Map44_Prg0 ) % ( NesHeader.rom_size << 1 ) );
    ROMBANK3 = ROMPAGE( ( ( Map44_Rom_Bank << 4 ) + 15 ) % ( NesHeader.rom_size << 1 ) );
  } else {
    ROMBANK0 = ROMPAGE( ( ( Map44_Rom_Bank << 4 ) + Map44_Prg0 ) % ( NesHeader.rom_size << 1 ) );
    ROMBANK1 = ROMPAGE( ( ( Map44_Rom_Bank << 4 ) + Map44_Prg1 ) % ( NesHeader.rom_size << 1 ) );
    ROMBANK2 = ROMPAGE( ( ( Map44_Rom_Bank << 4 ) + 14 ) % ( NesHeader.rom_size << 1 ) );
    ROMBANK3 = ROMPAGE( ( ( Map44_Rom_Bank << 4 ) + 15 ) % ( NesHeader.rom_size << 1 ) );
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 44 Set PPU Banks Function  映射器 44：设置 PPU 银行函数   */
/*-------------------------------------------------------------------*/
void Map44_Set_PPU_Banks()
{
  if ( NesHeader.v_rom_size > 0 )
  {
    if ( Map44_Chr_Swap() )
    { 
      PPUBANK[ 0 ] = VROMPAGE( ( ( Map44_Rom_Bank << 7 ) + Map44_Chr4 ) % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 1 ] = VROMPAGE( ( ( Map44_Rom_Bank << 7 ) + Map44_Chr5 ) % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 2 ] = VROMPAGE( ( ( Map44_Rom_Bank << 7 ) + Map44_Chr6 ) % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 3 ] = VROMPAGE( ( ( Map44_Rom_Bank << 7 ) + Map44_Chr7 ) % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 4 ] = VROMPAGE( ( ( Map44_Rom_Bank << 7 ) + Map44_Chr01 + 0 ) % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 5 ] = VROMPAGE( ( ( Map44_Rom_Bank << 7 ) + Map44_Chr01 + 1 ) % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 6 ] = VROMPAGE( ( ( Map44_Rom_Bank << 7 ) + Map44_Chr23 + 0 ) % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 7 ] = VROMPAGE( ( ( Map44_Rom_Bank << 7 ) + Map44_Chr23 + 1 ) % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();
    } else {
      PPUBANK[ 0 ] = VROMPAGE( ( ( Map44_Rom_Bank << 7 ) + Map44_Chr01 + 0 ) % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 1 ] = VROMPAGE( ( ( Map44_Rom_Bank << 7 ) + Map44_Chr01 + 1 ) % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 2 ] = VROMPAGE( ( ( Map44_Rom_Bank << 7 ) + Map44_Chr23 + 0 ) % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 3 ] = VROMPAGE( ( ( Map44_Rom_Bank << 7 ) + Map44_Chr23 + 1 ) % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 4 ] = VROMPAGE( ( ( Map44_Rom_Bank << 7 ) + Map44_Chr4 ) % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 5 ] = VROMPAGE( ( ( Map44_Rom_Bank << 7 ) + Map44_Chr5 ) % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 6 ] = VROMPAGE( ( ( Map44_Rom_Bank << 7 ) + Map44_Chr6 ) % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 7 ] = VROMPAGE( ( ( Map44_Rom_Bank << 7 ) + Map44_Chr7 ) % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();
    }
  }
}
