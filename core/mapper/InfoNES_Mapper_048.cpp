/*===================================================================*/
/*                                                                   */
/*                   Mapper 48 (Taito TC0190V)  映射器 48（Taito TC0190V） */
/*                                                                   */
/*===================================================================*/

uint8_t Map48_Regs[ 1 ];
uint8_t Map48_IRQ_Enable;
uint8_t Map48_IRQ_Cnt;

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 48  初始化映射器 48                            */
/*-------------------------------------------------------------------*/
void Map48_Init()
{
  /* Initialize Mapper  初始化映射器 */
  MapperInit = Map48_Init;

  /* Write to Mapper  写入映射器 */
  MapperWrite = Map48_Write;

  /* Write to SRAM  写入 SRAM */
  MapperSram = Map0_Sram;

  /* Write to APU  写入 APU */
  MapperApu = Map0_Apu;

  /* Read from APU  从 APU 读取 */
  MapperReadApu = Map0_ReadApu;

  /* Callback at VSync  场同步回调 */
  MapperVSync = Map0_VSync;

  /* Callback at HSync  行同步回调 */
  MapperHSync = Map48_HSync;

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
      PPUBANK[ page_number ] = VROMPAGE( page_number );
    InfoNES_SetupChr();
  }

  /* Initialize IRQ Registers  初始化 IRQ 寄存器 */
  Map48_Regs[ 0 ] = 0;
  Map48_IRQ_Enable = 0;
  Map48_IRQ_Cnt = 0;

  /* Set up wiring of the interrupt pin  设置中断引脚接线方式 */
  K6502_Set_Int_Wiring( 1, 1 ); 
}

/*-------------------------------------------------------------------*/
/*  Mapper 48 Write Function  映射器 48：写函数                      */
/*-------------------------------------------------------------------*/
void Map48_Write( uint16_t address, uint8_t byte_data )
{
  switch ( address )
  {
    case 0x8000:
      /* Name Table Mirroring  命名表镜像 */
      if ( !Map48_Regs[ 0 ] )
      {
        if ( byte_data & 0x40 )
        {
          InfoNES_Mirroring( 0 );
        } else {
          InfoNES_Mirroring( 1 );
        }
      }
      /* Set ROM Banks  设置 ROM 银行 */
      ROMBANK0 = ROMPAGE( byte_data % ( NesHeader.rom_size << 1 ) );
      break;

    case 0x8001:
      /* Set ROM Banks  设置 ROM 银行 */
      ROMBANK1 = ROMPAGE( byte_data % ( NesHeader.rom_size << 1 ) );
      break;  
 
    /* Set PPU Banks  设置 PPU 银行 */
    case 0x8002:
      PPUBANK[ 0 ] = VROMPAGE( ( ( byte_data << 1 ) + 0 ) % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 1 ] = VROMPAGE( ( ( byte_data << 1 ) + 1 ) % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();
      break;

    case 0x8003:
      PPUBANK[ 2 ] = VROMPAGE( ( ( byte_data << 1 ) + 0 ) % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 3 ] = VROMPAGE( ( ( byte_data << 1 ) + 1 ) % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();
      break;

    case 0xa000:
      PPUBANK[ 4 ] = VROMPAGE( ( ( byte_data << 1 ) + 0 ) % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();
      break;

    case 0xa001:
      PPUBANK[ 5 ] = VROMPAGE( ( ( byte_data << 1 ) + 0 ) % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();
      break;

    case 0xa002:
      PPUBANK[ 6 ] = VROMPAGE( ( ( byte_data << 1 ) + 0 ) % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();
      break;

    case 0xa003:
      PPUBANK[ 7 ] = VROMPAGE( ( ( byte_data << 1 ) + 0 ) % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();
      break;

    case 0xc000:
      Map48_IRQ_Cnt = byte_data;
      break;

    case 0xc001:
      Map48_IRQ_Enable = byte_data & 0x01;
      break;

    case 0xe000:
      /* Name Table Mirroring  命名表镜像 */
      if ( byte_data & 0x40 )
      {
        InfoNES_Mirroring( 0 );
      } else {
        InfoNES_Mirroring( 1 );
      }
      Map48_Regs[ 0 ] = 1;
      break;
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 48 H-Sync Function  映射器 48：行同步函数                 */
/*-------------------------------------------------------------------*/
void Map48_HSync()
{
/*
 *  Callback at HSync  行同步回调
 *
 */
  if ( Map48_IRQ_Enable )
  {
    if ( 0 <= PPU_Scanline && PPU_Scanline <= 239 )
    {
      if ( PPU_R1 & R1_SHOW_SCR || PPU_R1 & R1_SHOW_SP )
      {
        if ( Map48_IRQ_Cnt == 0xff )
        {
          IRQ_REQ;
          Map48_IRQ_Enable = 0;
        } else {
          Map48_IRQ_Cnt++;
        }
      }
    }
  }
}
