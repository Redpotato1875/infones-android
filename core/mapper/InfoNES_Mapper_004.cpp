/*===================================================================*/
/*                                                                   */
/*                       Mapper 4 (MMC3)  映射器 4（MMC3）           */
/*                                                                   */
/*===================================================================*/

uint8_t  Map4_Regs[ 8 ];
uint32_t Map4_Rom_Bank;
uint32_t Map4_Prg0, Map4_Prg1;
uint32_t Map4_Chr01, Map4_Chr23;
uint32_t Map4_Chr4, Map4_Chr5, Map4_Chr6, Map4_Chr7;

#define Map4_Chr_Swap()    ( Map4_Regs[ 0 ] & 0x80 )
#define Map4_Prg_Swap()    ( Map4_Regs[ 0 ] & 0x40 )

uint8_t Map4_IRQ_Enable;
uint8_t Map4_IRQ_Cnt;
uint8_t Map4_IRQ_Latch;
uint8_t Map4_IRQ_Request;
uint8_t Map4_IRQ_Present;
uint8_t Map4_IRQ_Present_Vbl;

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 4  初始化映射器 4                              */
/*-------------------------------------------------------------------*/
void Map4_Init()
{
  /* Initialize Mapper  初始化映射器 */
  MapperInit = Map4_Init;

  /* Write to Mapper  写入映射器 */
  MapperWrite = Map4_Write;

  /* Write to SRAM  写入 SRAM */
  MapperSram = Map0_Sram;

  /* Write to APU  写入 APU */
  MapperApu = Map0_Apu;

  /* Read from APU  从 APU 读取 */
  MapperReadApu = Map0_ReadApu;

  /* Callback at VSync  场同步回调 */
  MapperVSync = Map0_VSync;

  /* Callback at HSync  行同步回调 */
  MapperHSync = Map4_HSync;

  /* Callback at PPU  PPU 回调 */
  MapperPPU = Map0_PPU;

  /* Callback at Rendering Screen ( 1:BG, 0:Sprite )  渲染画面回调（1：背景，0：精灵） */
  MapperRenderScreen = Map0_RenderScreen;

  /* Set SRAM Banks  设置 SRAM 银行 */
  SRAMBANK = SRAM;

  /* Initialize State Registers  初始化状态寄存器 */
  for ( int page_number = 0; page_number < 8; page_number++ )
  {
    Map4_Regs[ page_number ] = 0x00;
  }

  /* Set ROM Banks  设置 ROM 银行 */
  Map4_Prg0 = 0;
  Map4_Prg1 = 1;
  Map4_Set_CPU_Banks();

  /* Set PPU Banks  设置 PPU 银行 */
  if ( NesHeader.v_rom_size > 0 )
  {
    Map4_Chr01 = 0;
    Map4_Chr23 = 2;
    Map4_Chr4  = 4;
    Map4_Chr5  = 5;
    Map4_Chr6  = 6;
    Map4_Chr7  = 7;
    Map4_Set_PPU_Banks();
  } else {
    Map4_Chr01 = Map4_Chr23 = 0;
    Map4_Chr4 = Map4_Chr5 = Map4_Chr6 = Map4_Chr7 = 0;
  }

  /* Initialize IRQ Registers  初始化 IRQ 寄存器 */
  Map4_IRQ_Enable = 0;
  Map4_IRQ_Cnt = 0;
  Map4_IRQ_Latch = 0;
  Map4_IRQ_Request = 0;
  Map4_IRQ_Present = 0;
  Map4_IRQ_Present_Vbl = 0;

  /* Set up wiring of the interrupt pin  设置中断引脚接线方式 */
  K6502_Set_Int_Wiring( 1, 1 ); 
}

/*-------------------------------------------------------------------*/
/*  Mapper 4 Write Function  映射器 4：写函数                        */
/*-------------------------------------------------------------------*/
void Map4_Write( uint16_t address, uint8_t byte_data )
{
  uint32_t bank_num;

  switch ( address & 0xe001 )
  {
    case 0x8000:
      Map4_Regs[ 0 ] = byte_data;
      Map4_Set_PPU_Banks();
      Map4_Set_CPU_Banks();
      break;

    case 0x8001:
      Map4_Regs[ 1 ] = byte_data;
      bank_num = Map4_Regs[ 1 ];

      switch ( Map4_Regs[ 0 ] & 0x07 )
      {
        /* Set PPU Banks  设置 PPU 银行 */
        case 0x00:
          if ( NesHeader.v_rom_size > 0 )
          {
            bank_num &= 0xfe;
            Map4_Chr01 = bank_num;
            Map4_Set_PPU_Banks();
          }
          break;

        case 0x01:
          if ( NesHeader.v_rom_size > 0 )
          {
            bank_num &= 0xfe;
            Map4_Chr23 = bank_num;
            Map4_Set_PPU_Banks();
          }
          break;

        case 0x02:
          if ( NesHeader.v_rom_size > 0 )
          {
            Map4_Chr4 = bank_num;
            Map4_Set_PPU_Banks();
          }
          break;

        case 0x03:
          if ( NesHeader.v_rom_size > 0 )
          {
            Map4_Chr5 = bank_num;
            Map4_Set_PPU_Banks();
          }
          break;

        case 0x04:
          if ( NesHeader.v_rom_size > 0 )
          {
            Map4_Chr6 = bank_num;
            Map4_Set_PPU_Banks();
          }
          break;

        case 0x05:
          if ( NesHeader.v_rom_size > 0 )
          {
            Map4_Chr7 = bank_num;
            Map4_Set_PPU_Banks();
          }
          break;

        /* Set ROM Banks  设置 ROM 银行 */
        case 0x06:
          Map4_Prg0 = bank_num;
          Map4_Set_CPU_Banks();
          break;

        case 0x07:
          Map4_Prg1 = bank_num;
          Map4_Set_CPU_Banks();
          break;
      }
      break;

    case 0xa000:
      Map4_Regs[ 2 ] = byte_data;

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
      Map4_Regs[ 3 ] = byte_data;
      break;

    case 0xc000:
      Map4_Regs[ 4 ] = byte_data;
      Map4_IRQ_Latch = byte_data;
      break;

    case 0xc001:
      Map4_Regs[ 5 ] = byte_data;
      if ( PPU_Scanline < 240 )
      {
          Map4_IRQ_Cnt |= 0x80;
          Map4_IRQ_Present = 0xff;
      } else {
          Map4_IRQ_Cnt |= 0x80;
          Map4_IRQ_Present_Vbl = 0xff;
          Map4_IRQ_Present = 0;
      }
      break;

    case 0xe000:
      Map4_Regs[ 6 ] = byte_data;
      Map4_IRQ_Enable = 0;
			Map4_IRQ_Request = 0;
      break;

    case 0xe001:
      Map4_Regs[ 7 ] = byte_data;
      Map4_IRQ_Enable = 1;
			Map4_IRQ_Request = 0;
      break;
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 4 H-Sync Function  映射器 4：行同步函数                   */
/*-------------------------------------------------------------------*/
void Map4_HSync()
{
/*
 *  Callback at HSync  行同步回调
 *
 */
  if ( ( 0 <= PPU_Scanline && PPU_Scanline <= 239 ) && 
       ( PPU_R1 & R1_SHOW_SCR || PPU_R1 & R1_SHOW_SP ) )
  {
		if( Map4_IRQ_Present_Vbl ) {
			Map4_IRQ_Cnt = Map4_IRQ_Latch;
			Map4_IRQ_Present_Vbl = 0;
		}
		if( Map4_IRQ_Present ) {
			Map4_IRQ_Cnt = Map4_IRQ_Latch;
			Map4_IRQ_Present = 0;
		} else if( Map4_IRQ_Cnt > 0 ) {
			Map4_IRQ_Cnt--;
		}

		if( Map4_IRQ_Cnt == 0 ) {
			if( Map4_IRQ_Enable ) {
				Map4_IRQ_Request = 0xFF;
			}
			Map4_IRQ_Present = 0xFF;
		}
	}
	if( Map4_IRQ_Request  ) {
		IRQ_REQ;
	}
}

/*-------------------------------------------------------------------*/
/*  Mapper 4 Set CPU Banks Function  映射器 4：设置 CPU 银行函数     */
/*-------------------------------------------------------------------*/
void Map4_Set_CPU_Banks()
{
  if ( Map4_Prg_Swap() )
  {
    ROMBANK0 = ROMLASTPAGE( 1 );
    ROMBANK1 = ROMPAGE( Map4_Prg1 % ( NesHeader.rom_size << 1 ) );
    ROMBANK2 = ROMPAGE( Map4_Prg0 % ( NesHeader.rom_size << 1 ) );
    ROMBANK3 = ROMLASTPAGE( 0 );
  } else {
    ROMBANK0 = ROMPAGE( Map4_Prg0 % ( NesHeader.rom_size << 1 ) );
    ROMBANK1 = ROMPAGE( Map4_Prg1 % ( NesHeader.rom_size << 1 ) );
    ROMBANK2 = ROMLASTPAGE( 1 );
    ROMBANK3 = ROMLASTPAGE( 0 );
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 4 Set PPU Banks Function  映射器 4：设置 PPU 银行函数     */
/*-------------------------------------------------------------------*/
void Map4_Set_PPU_Banks()
{
  if ( NesHeader.v_rom_size > 0 )
  {
    if ( Map4_Chr_Swap() )
    { 
      PPUBANK[ 0 ] = VROMPAGE( Map4_Chr4 % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 1 ] = VROMPAGE( Map4_Chr5 % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 2 ] = VROMPAGE( Map4_Chr6 % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 3 ] = VROMPAGE( Map4_Chr7 % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 4 ] = VROMPAGE( ( Map4_Chr01 + 0 ) % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 5 ] = VROMPAGE( ( Map4_Chr01 + 1 ) % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 6 ] = VROMPAGE( ( Map4_Chr23 + 0 ) % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 7 ] = VROMPAGE( ( Map4_Chr23 + 1 ) % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();
    } else {
      PPUBANK[ 0 ] = VROMPAGE( ( Map4_Chr01 + 0 ) % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 1 ] = VROMPAGE( ( Map4_Chr01 + 1 ) % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 2 ] = VROMPAGE( ( Map4_Chr23 + 0 ) % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 3 ] = VROMPAGE( ( Map4_Chr23 + 1 ) % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 4 ] = VROMPAGE( Map4_Chr4 % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 5 ] = VROMPAGE( Map4_Chr5 % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 6 ] = VROMPAGE( Map4_Chr6 % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 7 ] = VROMPAGE( Map4_Chr7 % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();
    }
  }
  else
  {
    if ( Map4_Chr_Swap() )
    { 
#if 0
      PPUBANK[ 0 ] = VRAMPAGE0( 0 );
      PPUBANK[ 1 ] = VRAMPAGE0( 1 );
      PPUBANK[ 2 ] = VRAMPAGE0( 2 );
      PPUBANK[ 3 ] = VRAMPAGE0( 3 );
      PPUBANK[ 4 ] = VRAMPAGE1( 0 );
      PPUBANK[ 5 ] = VRAMPAGE1( 1 );
      PPUBANK[ 6 ] = VRAMPAGE1( 2 );
      PPUBANK[ 7 ] = VRAMPAGE1( 3 );
#else
      PPUBANK[ 0 ] = CRAMPAGE( 0 );
      PPUBANK[ 1 ] = CRAMPAGE( 1 );
      PPUBANK[ 2 ] = CRAMPAGE( 2 );
      PPUBANK[ 3 ] = CRAMPAGE( 3 );
      PPUBANK[ 4 ] = CRAMPAGE( 4 );
      PPUBANK[ 5 ] = CRAMPAGE( 5 );
      PPUBANK[ 6 ] = CRAMPAGE( 6 );
      PPUBANK[ 7 ] = CRAMPAGE( 7 );
#endif
      InfoNES_SetupChr();
    } else {
#if 0
      PPUBANK[ 0 ] = VRAMPAGE1( 0 );
      PPUBANK[ 1 ] = VRAMPAGE1( 1 );
      PPUBANK[ 2 ] = VRAMPAGE1( 2 );
      PPUBANK[ 3 ] = VRAMPAGE1( 3 );
      PPUBANK[ 4 ] = VRAMPAGE0( 0 );
      PPUBANK[ 5 ] = VRAMPAGE0( 1 );
      PPUBANK[ 6 ] = VRAMPAGE0( 2 );
      PPUBANK[ 7 ] = VRAMPAGE0( 3 );
#else
      PPUBANK[ 0 ] = CRAMPAGE( 0 );
      PPUBANK[ 1 ] = CRAMPAGE( 1 );
      PPUBANK[ 2 ] = CRAMPAGE( 2 );
      PPUBANK[ 3 ] = CRAMPAGE( 3 );
      PPUBANK[ 4 ] = CRAMPAGE( 4 );
      PPUBANK[ 5 ] = CRAMPAGE( 5 );
      PPUBANK[ 6 ] = CRAMPAGE( 6 );
      PPUBANK[ 7 ] = CRAMPAGE( 7 );
#endif
      InfoNES_SetupChr();
    }
  }    
}
