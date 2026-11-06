/*===================================================================*/
/*                                                                   */
/*                        Mapper 112 (Pirates)  映射器 112（Pirates） */
/*                                                                   */
/*===================================================================*/

uint8_t  Map112_Regs[8];
uint32_t Map112_Prg0,Map112_Prg1;
uint32_t Map112_Chr01,Map112_Chr23,Map112_Chr4,Map112_Chr5,Map112_Chr6,Map112_Chr7;

uint8_t  Map112_IRQ_Enable;  /* IRQs enabled  允许 IRQ */
uint8_t  Map112_IRQ_Cnt;     /* IRQ scanline counter, decreasing  IRQ 扫描行计数器（递减） */
uint8_t  Map112_IRQ_Latch;   /* IRQ scanline counter latch  IRQ 扫描行计数锁存 */

#define Map112_Chr_Swap()  ( Map112_Regs[0] & 0x80 )
#define Map112_Prg_Swap()  ( Map112_Regs[0] & 0x40 )

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 112  初始化映射器 112                          */
/*-------------------------------------------------------------------*/
void Map112_Init()
{
  /* Initialize Mapper  初始化映射器 */
  MapperInit = Map112_Init;

  /* Write to Mapper  写入映射器 */
  MapperWrite = Map112_Write;

  /* Write to SRAM  写入 SRAM */
  MapperSram = Map0_Sram;

  /* Write to APU  写入 APU */
  MapperApu = Map0_Apu;

  /* Read from APU  从 APU 读取 */
  MapperReadApu = Map0_ReadApu;

  /* Callback at VSync  场同步回调 */
  MapperVSync = Map0_VSync;

  /* Callback at HSync  行同步回调 */
  MapperHSync = Map112_HSync;

  /* Callback at PPU  PPU 回调 */
  MapperPPU = Map0_PPU;

  /* Callback at Rendering Screen ( 1:BG, 0:Sprite )  渲染画面回调（1：背景，0：精灵） */
  MapperRenderScreen = Map0_RenderScreen;

  /* Set SRAM Banks  设置 SRAM 银行 */
  SRAMBANK = SRAM;

  /* clear registers FIRST!!!  务必先清空寄存器！ */
  for(int i = 0; i < 8; i++) 
  {
    Map112_Regs[i] = 0x00;
  }

  /* set CPU bank pointers  设置 CPU 银行指针 */
  Map112_Prg0 = 0;
  Map112_Prg1 = 1;
  Map112_Set_CPU_Banks();

  /* set VROM banks  设置 VROM 银行 */
  if ( NesHeader.v_rom_size > 0 )
  {
    Map112_Chr01 = 0;
    Map112_Chr23 = 2;
    Map112_Chr4  = 4;
    Map112_Chr5  = 5;
    Map112_Chr6  = 6;
    Map112_Chr7  = 7;
    Map112_Set_PPU_Banks();
  }
  else
  {
    Map112_Chr01 = Map112_Chr23 = Map112_Chr4 = Map112_Chr5 = Map112_Chr6 = Map112_Chr7 = 0;
  }

  Map112_IRQ_Enable = 0;
  Map112_IRQ_Cnt = 0;
  Map112_IRQ_Latch = 0;

  /* Set up wiring of the interrupt pin  设置中断引脚接线方式 */
  K6502_Set_Int_Wiring( 1, 1 ); 
}

/*-------------------------------------------------------------------*/
/*  Mapper 112 Write Function  映射器 112：写函数                    */
/*-------------------------------------------------------------------*/
void Map112_Write( uint16_t address, uint8_t byte_data )
{
  uint32_t bank_num;

  /* Set ROM Banks  设置 ROM 银行 */
  switch( address & 0xE001 )
  {
    case 0x8000:
      Map112_Regs[0] = byte_data;
      Map112_Set_PPU_Banks();
      Map112_Set_CPU_Banks();
      break;

    case 0xA000:
      Map112_Regs[1] = byte_data;
      bank_num = Map112_Regs[1];
      switch ( Map112_Regs[0] & 0x07 )
      {
        case 0x02:
	  if ( NesHeader.v_rom_size > 0 )
	  {
	    bank_num &= 0xfe;
	    Map112_Chr01 = bank_num;
	    Map112_Set_PPU_Banks();
	  }
	  break;

        case 0x03:
	  if ( NesHeader.v_rom_size > 0 )
	  {
	    bank_num &= 0xfe;
	    Map112_Chr23 = bank_num;
	    Map112_Set_PPU_Banks();
	  }
	  break;

        case 0x04:
	  if ( NesHeader.v_rom_size > 0 )
          {
	    Map112_Chr4 = bank_num;
	    Map112_Set_PPU_Banks();
	  }
	  break;

        case 0x05:
	  if ( NesHeader.v_rom_size > 0 )
          {
	    Map112_Chr5 = bank_num;
	    Map112_Set_PPU_Banks();
	  }
	  break;

        case 0x06:
	  if ( NesHeader.v_rom_size > 0 )
          {
	    Map112_Chr6 = bank_num;
	    Map112_Set_PPU_Banks();
          }
	  break;

        case 0x07:
	  if ( NesHeader.v_rom_size > 0 )
          {
	    Map112_Chr7 = bank_num;
	    Map112_Set_PPU_Banks();
	  }
	  break;

        case 0x00:
	  Map112_Prg0 = bank_num;
	  Map112_Set_CPU_Banks();
	  break;

        case 0x01:
	  Map112_Prg1 = bank_num;
	  Map112_Set_CPU_Banks();
	  break;
      }
      break;

    case 0x8001:
      Map112_Regs[2] = byte_data;
      if ( !ROM_FourScr )
      {
	if ( byte_data & 0x01 )
        {
	  InfoNES_Mirroring( 1 );   /* Vertical  垂直 */
	}
	else
        {
	  InfoNES_Mirroring( 0 );   /* Horizontal  水平 */
	}
      }
      break;

    case 0xA001:
      Map112_Regs[3] = byte_data;
      break;

    case 0xC000:
      Map112_Regs[4] = byte_data;
      Map112_IRQ_Cnt = Map112_Regs[4];
      break;

    case 0xC001:
      Map112_Regs[5] = byte_data;
      Map112_IRQ_Latch = Map112_Regs[5];
      break;

    case 0xE000:
      Map112_Regs[6] = byte_data;
      Map112_IRQ_Enable = 0;

      if ( byte_data )
      {
	InfoNES_Mirroring( 0 );   /* Horizontal  水平 */
      }
      else
      {
	InfoNES_Mirroring( 1 );   /* Vertical  垂直 */
      }
      break;

    case 0xE001:
      Map112_Regs[7] = byte_data;
      Map112_IRQ_Enable = 1;
      break;
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 112 H-Sync Function  映射器 112：行同步函数               */
/*-------------------------------------------------------------------*/
void Map112_HSync()
{
/*
 *  Callback at HSync  行同步回调
 *
 */
  if ( Map112_IRQ_Enable )
  {
    if ( 0 <= PPU_Scanline && PPU_Scanline <= 239 )
    {
      if ( PPU_R1 & R1_SHOW_SCR || PPU_R1 & R1_SHOW_SP )
      {
        if ( !( Map112_IRQ_Cnt-- ) )
        {
          Map112_IRQ_Cnt = Map112_IRQ_Latch;
          IRQ_REQ;
        }
      }
    }
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 112 Set CPU Banks Function  映射器 112：设置 CPU 银行函数 */
/*-------------------------------------------------------------------*/
void Map112_Set_CPU_Banks()
{
  /* Set ROM Banks  设置 ROM 银行 */
  if ( Map112_Prg_Swap() )
  {
    ROMBANK0 = ROMLASTPAGE( 1 );
    ROMBANK1 = ROMPAGE( Map112_Prg1 % ( NesHeader.rom_size << 1 ) );
    ROMBANK2 = ROMPAGE( Map112_Prg0 % ( NesHeader.rom_size << 1 ) );
    ROMBANK3 = ROMLASTPAGE( 0 );
  }
  else
  {
    ROMBANK0 = ROMPAGE( Map112_Prg0 % ( NesHeader.rom_size << 1 ) );
    ROMBANK1 = ROMPAGE( Map112_Prg1 % ( NesHeader.rom_size << 1 ) );
    ROMBANK2 = ROMLASTPAGE( 1 );
    ROMBANK3 = ROMLASTPAGE( 0 );
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 112 Set PPU Banks Function  映射器 112：设置 PPU 银行函数 */
/*-------------------------------------------------------------------*/
void Map112_Set_PPU_Banks()
{
  if ( NesHeader.v_rom_size > 0 )
  {
    if ( Map112_Chr_Swap() )
    {
      PPUBANK[ 0 ] = VROMPAGE( Map112_Chr4 % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 1 ] = VROMPAGE( Map112_Chr5 % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 2 ] = VROMPAGE( Map112_Chr6 % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 3 ] = VROMPAGE( Map112_Chr7 % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 4 ] = VROMPAGE( ( Map112_Chr01 + 0 ) % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 5 ] = VROMPAGE( ( Map112_Chr01 + 1 ) % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 6 ] = VROMPAGE( ( Map112_Chr23 + 0 ) % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 7 ] = VROMPAGE( ( Map112_Chr23 + 1 ) % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();
    } 
    else 
    {
      PPUBANK[ 0 ] = VROMPAGE( ( Map112_Chr01 + 0 ) % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 1 ] = VROMPAGE( ( Map112_Chr01 + 1 ) % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 2 ] = VROMPAGE( ( Map112_Chr23 + 0 ) % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 3 ] = VROMPAGE( ( Map112_Chr23 + 1 ) % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 4 ] = VROMPAGE( Map112_Chr4 % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 5 ] = VROMPAGE( Map112_Chr5 % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 6 ] = VROMPAGE( Map112_Chr6 % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 7 ] = VROMPAGE( Map112_Chr7 % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();
    }
  }
}
