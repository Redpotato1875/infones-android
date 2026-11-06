/*===================================================================*/
/*                                                                   */
/*                      Mapper 182  (Pirates)  映射器 182（盗版卡）  */
/*                                                                   */
/*===================================================================*/

uint8_t Map182_Regs[1];
uint8_t Map182_IRQ_Enable;
uint8_t Map182_IRQ_Cnt;

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 182  初始化映射器 182                          */
/*-------------------------------------------------------------------*/
void Map182_Init()
{
  /* Initialize Mapper  初始化映射器 */
  MapperInit = Map182_Init;

  /* Write to Mapper  写入映射器 */
  MapperWrite = Map182_Write;

  /* Write to SRAM  写入 SRAM */
  MapperSram = Map0_Sram;

  /* Write to APU  写入 APU */
  MapperApu = Map0_Apu;

  /* Read from APU  从 APU 读取 */
  MapperReadApu = Map0_ReadApu;

  /* Callback at VSync  场同步回调 */
  MapperVSync = Map0_VSync;

  /* Callback at HSync  行同步回调 */
  MapperHSync = Map182_HSync;

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
  Map182_Regs[0] = 0;
  Map182_IRQ_Enable = 0;
  Map182_IRQ_Cnt = 0;

  /* Set up wiring of the interrupt pin  设置中断引脚接线方式 */
  K6502_Set_Int_Wiring( 1, 1 ); 
}

/*-------------------------------------------------------------------*/
/*  Mapper 182 Write Function  映射器 182：写函数                    */
/*-------------------------------------------------------------------*/
void Map182_Write( uint16_t address, uint8_t byte_data )
{
  switch( address & 0xF003 )
  {
    /* Name Table Mirroring  命名表镜像 */
    case 0x8001:
      if ( byte_data & 0x01 )
      {
        InfoNES_Mirroring( 0 );   /* Horizontal  水平 */
      }
      else
      {
        InfoNES_Mirroring( 1 );   /* Vertical  垂直 */
      }
      break;

    case 0xA000:
      Map182_Regs[0] = byte_data & 0x07;
      break;

    case 0xC000:
      switch( Map182_Regs[0] )
      { 
        /* Set PPU Banks  设置 PPU 银行 */
        case 0x00:
          PPUBANK[ 0 ] = VROMPAGE( ( ( byte_data & 0xFE ) + 0 ) % ( NesHeader.v_rom_size << 3 ) );
          PPUBANK[ 1 ] = VROMPAGE( ( ( byte_data & 0xFE ) + 1 ) % ( NesHeader.v_rom_size << 3 ) );
          InfoNES_SetupChr();
          break;

        case 0x01:
          PPUBANK[ 5 ] = VROMPAGE( byte_data % ( NesHeader.v_rom_size << 3 ) );
          InfoNES_SetupChr();
          break;

        case 0x02:
          PPUBANK[ 2 ] = VROMPAGE( ( ( byte_data & 0xFE ) + 0 ) % ( NesHeader.v_rom_size << 3 ) );
          PPUBANK[ 3 ] = VROMPAGE( ( ( byte_data & 0xFE ) + 1 ) % ( NesHeader.v_rom_size << 3 ) );
          InfoNES_SetupChr();
          break;

        case 0x03:
          PPUBANK[ 7 ] = VROMPAGE( byte_data % ( NesHeader.v_rom_size << 3 ) );
          InfoNES_SetupChr();
          break;

        /* Set ROM Banks  设置 ROM 银行 */
        case 0x04:
          ROMBANK0 = ROMPAGE( byte_data % ( NesHeader.rom_size << 1 ) );
          break;

        case 0x05:
          ROMBANK1 = ROMPAGE( byte_data % ( NesHeader.rom_size << 1 ) );
          break;

        /* Set PPU Banks  设置 PPU 银行 */
        case 0x06:
          PPUBANK[ 4 ] = VROMPAGE( byte_data % ( NesHeader.v_rom_size << 3 ) );
          InfoNES_SetupChr();
          break;

        case 0x07:
          PPUBANK[ 6 ] = VROMPAGE( byte_data % ( NesHeader.v_rom_size << 3 ) );
          InfoNES_SetupChr();
          break;
      }
      break;

   case 0xE003:
     Map182_IRQ_Cnt = byte_data;
     Map182_IRQ_Enable = byte_data;
     break;
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 182 H-Sync Function  映射器 182：行同步函数               */
/*-------------------------------------------------------------------*/
void Map182_HSync()
{
/*
 *  Callback at HSync  行同步回调
 *
 */
  if ( Map182_IRQ_Enable )
  {
    if ( 0 <= PPU_Scanline && PPU_Scanline <= 240 )
    {
      if ( PPU_R1 & R1_SHOW_SCR || PPU_R1 & R1_SHOW_SP )
      {
        if ( !( --Map182_IRQ_Cnt ) )
        {
          Map182_IRQ_Cnt = 0;
          Map182_IRQ_Enable = 0;
          IRQ_REQ;
        }
      }
    }
  }
}
