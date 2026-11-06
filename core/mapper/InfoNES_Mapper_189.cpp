/*===================================================================*/
/*                                                                   */
/*                       Mapper 189 (Pirates)  映射器 189（Pirates） */
/*                                                                   */
/*===================================================================*/

uint8_t Map189_Regs[ 1 ];
uint8_t Map189_IRQ_Cnt;
uint8_t Map189_IRQ_Latch;
uint8_t Map189_IRQ_Enable;

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 189  初始化映射器 189                          */
/*-------------------------------------------------------------------*/
void Map189_Init()
{
  /* Initialize Mapper  初始化映射器 */
  MapperInit = Map189_Init;

  /* Write to Mapper  写入映射器 */
  MapperWrite = Map189_Write;

  /* Write to SRAM  写入 SRAM */
  MapperSram = Map0_Sram;

  /* Write to APU  写入 APU */
  MapperApu = Map189_Apu;

  /* Read from APU  从 APU 读取 */
  MapperReadApu = Map0_ReadApu;

  /* Callback at VSync  场同步回调 */
  MapperVSync = Map0_VSync;

  /* Callback at HSync  行同步回调 */
  MapperHSync = Map189_HSync;

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
    for ( int page_number = 0; page_number < 8; ++page_number )
    {
      PPUBANK[ page_number ] = VROMPAGE( page_number );
    }
    InfoNES_SetupChr();
  }

  /* Initialize IRQ registers  初始化 IRQ 寄存器 */
  Map189_IRQ_Cnt = 0;
  Map189_IRQ_Latch = 0;
  Map189_IRQ_Enable = 0;

  /* Set up wiring of the interrupt pin  设置中断引脚接线方式 */
  K6502_Set_Int_Wiring( 1, 1 ); 
}

/*-------------------------------------------------------------------*/
/*  Mapper 189 Write to Apu Function  映射器 189：写 APU 函数        */
/*-------------------------------------------------------------------*/
void Map189_Apu( uint16_t address, uint8_t byte_data )
{
  if ( address >= 0x4100 && address <= 0x41FF )
  {
    byte_data = ( byte_data & 0x30 ) >> 4;
    ROMBANK0 = ROMPAGE( ( byte_data * 4 + 0 ) % ( NesHeader.rom_size << 1 ) );
    ROMBANK1 = ROMPAGE( ( byte_data * 4 + 1 ) % ( NesHeader.rom_size << 1 ) );
    ROMBANK2 = ROMPAGE( ( byte_data * 4 + 2 ) % ( NesHeader.rom_size << 1 ) );
    ROMBANK3 = ROMPAGE( ( byte_data * 4 + 3 ) % ( NesHeader.rom_size << 1 ) );
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 189 Write Function  映射器 189：写函数                    */
/*-------------------------------------------------------------------*/
void Map189_Write( uint16_t address, uint8_t byte_data )
{
  /* Set ROM Banks  设置 ROM 银行 */
  switch( address )
  {
    case 0x8000:
      Map189_Regs[0] = byte_data;
      break;

    case 0x8001:
      switch( Map189_Regs[0] )
      {
        case 0x40:
	  PPUBANK[ 0 ] = VROMPAGE( ( byte_data + 0 ) % ( NesHeader.v_rom_size << 3 ) );
	  PPUBANK[ 1 ] = VROMPAGE( ( byte_data + 1 ) % ( NesHeader.v_rom_size << 3 ) );
	  InfoNES_SetupChr();
	  break;

        case 0x41:
	  PPUBANK[ 2 ] = VROMPAGE( ( byte_data + 0 ) % ( NesHeader.v_rom_size << 3 ) );
	  PPUBANK[ 3 ] = VROMPAGE( ( byte_data + 1 ) % ( NesHeader.v_rom_size << 3 ) );
	  InfoNES_SetupChr();
	  break;

        case 0x42:
	  PPUBANK[ 4 ] = VROMPAGE( byte_data % ( NesHeader.v_rom_size << 3 ) );
	  InfoNES_SetupChr();
	  break;

        case 0x43:
	  PPUBANK[ 5 ] = VROMPAGE( byte_data % ( NesHeader.v_rom_size << 3 ) );
	  InfoNES_SetupChr();
	  break;

        case 0x44:
	  PPUBANK[ 6 ] = VROMPAGE( byte_data % ( NesHeader.v_rom_size << 3 ) );
	  InfoNES_SetupChr();
	  break;

        case 0x45:
	  PPUBANK[ 7 ] = VROMPAGE( byte_data % ( NesHeader.v_rom_size << 3 ) );
	  InfoNES_SetupChr();
	  break;

        case 0x46:
	  ROMBANK2 = ROMPAGE( byte_data % ( NesHeader.rom_size << 1 ) );
	  break;  

        case 0x47:
	  ROMBANK1 = ROMPAGE( byte_data % ( NesHeader.rom_size << 1 ) );
	  break;  
      }
      break;

    case 0xC000:
      Map189_IRQ_Cnt = byte_data;
      break;

    case 0xC001:
      Map189_IRQ_Latch = byte_data;
      break;

    case 0xE000:
      Map189_IRQ_Enable = 0;
      break;

    case 0xE001:
      Map189_IRQ_Enable = 1;
      break;
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 189 H-Sync Function  映射器 189：行同步函数               */
/*-------------------------------------------------------------------*/
void Map189_HSync()
{
/*
 *  Callback at HSync  行同步回调
 *
 */
  if ( Map189_IRQ_Enable )
  {
    if ( 0 <= PPU_Scanline && PPU_Scanline <= 239 )
    {
      if ( PPU_R1 & R1_SHOW_SCR || PPU_R1 & R1_SHOW_SP )
      {
        if ( !( --Map189_IRQ_Cnt ) )
        {
          Map189_IRQ_Cnt = Map189_IRQ_Latch;
          IRQ_REQ;
        }
      }
    }
  }
}
