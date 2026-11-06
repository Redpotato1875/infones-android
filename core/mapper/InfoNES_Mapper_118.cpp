/*===================================================================*/
/*                                                                   */
/*                       Mapper 118 (Others)  映射器 118（Others）   */
/*                                                                   */
/*===================================================================*/

uint8_t  Map118_Regs[ 8 ];
uint32_t Map118_Prg0, Map118_Prg1;
uint32_t Map118_Chr0, Map118_Chr1, Map118_Chr2, Map118_Chr3;
uint32_t Map118_Chr4, Map118_Chr5, Map118_Chr6, Map118_Chr7;

uint8_t Map118_IRQ_Enable;
uint8_t Map118_IRQ_Cnt;
uint8_t Map118_IRQ_Latch;

#define Map118_Chr_Swap()    ( Map118_Regs[ 0 ] & 0x80 )
#define Map118_Prg_Swap()    ( Map118_Regs[ 0 ] & 0x40 )

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 118  初始化映射器 118                          */
/*-------------------------------------------------------------------*/
void Map118_Init()
{
  /* Initialize Mapper  初始化映射器 */
  MapperInit = Map118_Init;

  /* Write to Mapper  写入映射器 */
  MapperWrite = Map118_Write;

  /* Write to SRAM  写入 SRAM */
  MapperSram = Map0_Sram;

  /* Write to APU  写入 APU */
  MapperApu = Map0_Apu;

  /* Read from APU  从 APU 读取 */
  MapperReadApu = Map0_ReadApu;

  /* Callback at VSync  场同步回调 */
  MapperVSync = Map0_VSync;

  /* Callback at HSync  行同步回调 */
  MapperHSync = Map118_HSync;

  /* Callback at PPU  PPU 回调 */
  MapperPPU = Map0_PPU;

  /* Callback at Rendering Screen ( 1:BG, 0:Sprite )  渲染画面回调（1：背景，0：精灵） */
  MapperRenderScreen = Map0_RenderScreen;

  /* Set SRAM Banks  设置 SRAM 银行 */
  SRAMBANK = SRAM;

  /* Initialize State Registers  初始化状态寄存器 */
  for ( int page_number = 0; page_number < 8; page_number++)
  {
    Map118_Regs[ page_number ] = 0x00;
  }

  /* Set ROM Banks  设置 ROM 银行 */
  Map118_Prg0 = 0;
  Map118_Prg1 = 1;
  Map118_Set_CPU_Banks();

  /* Set PPU Banks  设置 PPU 银行 */
  if ( NesHeader.v_rom_size > 0 )
  {
    Map118_Chr0 = 0;
    Map118_Chr1 = 1;
    Map118_Chr2 = 2;
    Map118_Chr3 = 3;
    Map118_Chr4 = 4;
    Map118_Chr5 = 5;
    Map118_Chr6 = 6;
    Map118_Chr7 = 7;
    Map118_Set_PPU_Banks();
  } else {
    Map118_Chr0 = Map118_Chr1 = Map118_Chr2 = Map118_Chr3 = 0;
    Map118_Chr4 = Map118_Chr5 = Map118_Chr6 = Map118_Chr7 = 0;
  }

  /* Initialize IRQ Registers  初始化 IRQ 寄存器 */
  Map118_IRQ_Enable = 0;
  Map118_IRQ_Cnt = 0;
  Map118_IRQ_Latch = 0;

  /* Set up wiring of the interrupt pin  设置中断引脚接线方式 */
  K6502_Set_Int_Wiring( 1, 1 ); 
}

/*-------------------------------------------------------------------*/
/*  Mapper 118 Write Function  映射器 118：写函数                    */
/*-------------------------------------------------------------------*/
void Map118_Write( uint16_t address, uint8_t byte_data )
{
  uint32_t bank_num;

  switch ( address & 0xe001 )
  {
    case 0x8000:
      Map118_Regs[ 0 ] = byte_data;
      Map118_Set_PPU_Banks();
      Map118_Set_CPU_Banks();
      break;

    case 0x8001:
      Map118_Regs[ 1 ] = byte_data;
      bank_num = Map118_Regs[ 1 ];

      /* Name Table Mirroring  命名表镜像 */
      if ( ( Map118_Regs[ 0 ] & 0x07 ) < 6 )
      {
        if ( byte_data & 0x80 )
        {
          InfoNES_Mirroring( 3 );
        } else {
          InfoNES_Mirroring( 2 );
        }
      }

      switch ( Map118_Regs[ 0 ] & 0x07 )
      {
        /* Set PPU Banks  设置 PPU 银行 */
        case 0x00:
          if ( NesHeader.v_rom_size > 0 )
          {
            bank_num &= 0xfe;
            Map118_Chr0 = bank_num;
            Map118_Chr1 = bank_num + 1;
            Map118_Set_PPU_Banks();
          }
          break;

        case 0x01:
          if ( NesHeader.v_rom_size > 0 )
          {
            bank_num &= 0xfe;
            Map118_Chr2 = bank_num;
            Map118_Chr3 = bank_num + 1;
            Map118_Set_PPU_Banks();
          }
          break;

        case 0x02:
          if ( NesHeader.v_rom_size > 0 )
          {
            Map118_Chr4 = bank_num;
            Map118_Set_PPU_Banks();
          }
          break;

        case 0x03:
          if ( NesHeader.v_rom_size > 0 )
          {
            Map118_Chr5 = bank_num;
            Map118_Set_PPU_Banks();
          }
          break;

        case 0x04:
          if ( NesHeader.v_rom_size > 0 )
          {
            Map118_Chr6 = bank_num;
            Map118_Set_PPU_Banks();
          }
          break;

        case 0x05:
          if ( NesHeader.v_rom_size > 0 )
          {
            Map118_Chr7 = bank_num;
            Map118_Set_PPU_Banks();
          }
          break;

        /* Set ROM Banks  设置 ROM 银行 */
        case 0x06:
          Map118_Prg0 = bank_num;
          Map118_Set_CPU_Banks();
          break;

        case 0x07:
          Map118_Prg1 = bank_num;
          Map118_Set_CPU_Banks();
          break;
      }
      break;

    case 0xc000:
      Map118_Regs[ 4 ] = byte_data;
      Map118_IRQ_Cnt = Map118_Regs[ 4 ];
      break;

    case 0xc001:
      Map118_Regs[ 5 ] = byte_data;
      Map118_IRQ_Latch = Map118_Regs[ 5 ];
      break;

    case 0xe000:
      Map118_Regs[ 6 ] = byte_data;
      Map118_IRQ_Enable = 0;
      break;

    case 0xe001:
      Map118_Regs[ 7 ] = byte_data;
      Map118_IRQ_Enable = 1;
      break;
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 118 H-Sync Function  映射器 118：行同步函数               */
/*-------------------------------------------------------------------*/
void Map118_HSync()
{
/*
 *  Callback at HSync  行同步回调
 *
 */
  if ( Map118_IRQ_Enable )
  {
    if ( 0 <= PPU_Scanline && PPU_Scanline <= 239 )
    {
      if ( PPU_R1 & R1_SHOW_SCR || PPU_R1 & R1_SHOW_SP )
      {
        if ( !( Map118_IRQ_Cnt-- ) )
        {
          Map118_IRQ_Cnt = Map118_IRQ_Latch;
          IRQ_REQ;
        }
      }
    }
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 118 Set CPU Banks Function  映射器 118：设置 CPU 银行函数 */
/*-------------------------------------------------------------------*/
void Map118_Set_CPU_Banks()
{
  if ( Map118_Prg_Swap() )
  {
    ROMBANK0 = ROMLASTPAGE( 1 );
    ROMBANK1 = ROMPAGE( Map118_Prg1 % ( NesHeader.rom_size << 1 ) );
    ROMBANK2 = ROMPAGE( Map118_Prg0 % ( NesHeader.rom_size << 1 ) );
    ROMBANK3 = ROMLASTPAGE( 0 );
  } else {
    ROMBANK0 = ROMPAGE( Map118_Prg0 % ( NesHeader.rom_size << 1 ) );
    ROMBANK1 = ROMPAGE( Map118_Prg1 % ( NesHeader.rom_size << 1 ) );
    ROMBANK2 = ROMLASTPAGE( 1 );
    ROMBANK3 = ROMLASTPAGE( 0 );
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 118 Set PPU Banks Function  映射器 118：设置 PPU 银行函数 */
/*-------------------------------------------------------------------*/
void Map118_Set_PPU_Banks()
{
  if ( NesHeader.v_rom_size > 0 )
  {
    if ( Map118_Chr_Swap() )
    { 
      PPUBANK[ 0 ] = VROMPAGE( Map118_Chr4 % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 1 ] = VROMPAGE( Map118_Chr5 % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 2 ] = VROMPAGE( Map118_Chr6 % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 3 ] = VROMPAGE( Map118_Chr7 % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 4 ] = VROMPAGE( Map118_Chr0 % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 5 ] = VROMPAGE( Map118_Chr1 % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 6 ] = VROMPAGE( Map118_Chr2 % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 7 ] = VROMPAGE( Map118_Chr3 % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();
    } else {
      PPUBANK[ 0 ] = VROMPAGE( Map118_Chr0 % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 1 ] = VROMPAGE( Map118_Chr1 % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 2 ] = VROMPAGE( Map118_Chr2 % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 3 ] = VROMPAGE( Map118_Chr3 % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 4 ] = VROMPAGE( Map118_Chr4 % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 5 ] = VROMPAGE( Map118_Chr5 % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 6 ] = VROMPAGE( Map118_Chr6 % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 7 ] = VROMPAGE( Map118_Chr7 % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();
    }
  }
}
