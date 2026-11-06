/*===================================================================*/
/*                                                                   */
/*                 Mapper 33 (Taito TC0190/TC0350)  映射器 33（Taito TC0190/TC0350） */
/*                                                                   */
/*===================================================================*/

uint8_t Map33_Regs[ 8 ];
uint8_t Map33_Switch;

uint8_t Map33_IRQ_Enable;
uint8_t Map33_IRQ_Cnt;

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 33  初始化映射器 33                            */
/*-------------------------------------------------------------------*/
void Map33_Init()
{
  /* Initialize Mapper  初始化映射器 */
  MapperInit = Map33_Init;

  /* Write to Mapper  写入映射器 */
  MapperWrite = Map33_Write;

  /* Write to SRAM  写入 SRAM */
  MapperSram = Map0_Sram;

  /* Write to APU  写入 APU */
  MapperApu = Map0_Apu;

  /* Read from APU  从 APU 读取 */
  MapperReadApu = Map0_ReadApu;

  /* Callback at VSync  场同步回调 */
  MapperVSync = Map0_VSync;

  /* Callback at HSync  行同步回调 */
  MapperHSync = Map0_HSync;

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
      Map33_Regs[ page_number ] = page_number;
    }
    InfoNES_SetupChr();
  } 
  else 
  {
    for ( int page_number = 0; page_number < 8; ++page_number )
    {
      Map33_Regs[ page_number ] = 0;
    }
  }

  /* Initialize State Registers  初始化状态寄存器 */
  Map33_Switch = 0;
  Map33_IRQ_Enable = 0;
  Map33_IRQ_Cnt = 0;

  /* Set up wiring of the interrupt pin  设置中断引脚接线方式 */
  K6502_Set_Int_Wiring( 1, 1 ); 
}

/*-------------------------------------------------------------------*/
/*  Mapper 33 Write Function  映射器 33：写函数                      */
/*-------------------------------------------------------------------*/
void Map33_Write( uint16_t address, uint8_t byte_data )
{
  /* Set ROM Banks  设置 ROM 银行 */
  switch ( address )
  {
    case 0x8000:
      byte_data %= ( NesHeader.rom_size << 1 );
      ROMBANK0 = ROMPAGE( byte_data );
      break;

    case 0x8001:
      byte_data %= ( NesHeader.rom_size << 1 );
      ROMBANK1 = ROMPAGE( byte_data );
      break;

    case 0x8002:
      Map33_Regs[ 0 ] = byte_data * 2;
      Map33_Regs[ 1 ] = byte_data * 2 + 1;

      PPUBANK[ 0 ] = VROMPAGE( Map33_Regs[ 0 ] % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 1 ] = VROMPAGE( Map33_Regs[ 1 ] % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();
      break;

    case 0x8003:
      Map33_Regs[ 2 ] = byte_data * 2;
      Map33_Regs[ 3 ] = byte_data * 2 + 1;

      PPUBANK[ 2 ] = VROMPAGE( Map33_Regs[ 2 ] % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 3 ] = VROMPAGE( Map33_Regs[ 3 ] % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();
      break;

    case 0xa000:
      Map33_Regs[ 4 ] = byte_data;
      PPUBANK[ 4 ] = VROMPAGE( Map33_Regs[ 4 ] % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();
      break;

    case 0xa001:
      Map33_Regs[ 5 ] = byte_data;
      PPUBANK[ 5 ] = VROMPAGE( Map33_Regs[ 5 ] % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();
      break;

    case 0xa002:
      Map33_Regs[ 6 ] = byte_data;
      PPUBANK[ 6 ] = VROMPAGE( Map33_Regs[ 6 ] % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();
      break;

    case 0xa003:
      Map33_Regs[ 7 ] = byte_data;
      PPUBANK[ 7 ] = VROMPAGE( Map33_Regs[ 7 ] % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();
      break;

    case 0xc000:
      Map33_IRQ_Cnt = byte_data;
      break;

    case 0xc001:
    case 0xc002:
    case 0xe001:
    case 0xe002:
      if ( Map33_IRQ_Cnt == byte_data )
      {
        Map33_IRQ_Enable = 0xff;
      } else {
        Map33_IRQ_Enable = byte_data;
      }
      break;

    case 0xe000:
      if ( byte_data & 0x40 )
      {
        InfoNES_Mirroring( 0 );
      } else {
        InfoNES_Mirroring( 1 );
      }
      break;
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 33 H-Sync Function  映射器 33：行同步函数                 */
/*-------------------------------------------------------------------*/
void Map33_HSync()
{
/*
 *  Callback at HSync  行同步回调
 *
 */
  if ( Map33_IRQ_Enable )
  {
    if ( Map33_IRQ_Enable == 0xff )
    {
      if ( PPU_Scanline == (uint16_t)( 0xff - Map33_IRQ_Cnt ) )
      {
        IRQ_REQ;
        Map33_IRQ_Cnt = 0;
        Map33_IRQ_Enable = 0;
      }
    } else {
      if ( Map33_IRQ_Cnt == 0xff )
      {
        IRQ_REQ;
        Map33_IRQ_Cnt = 0;
        Map33_IRQ_Enable = 0;
      } else {
        Map33_IRQ_Cnt++;
      }
    }
  }
}
