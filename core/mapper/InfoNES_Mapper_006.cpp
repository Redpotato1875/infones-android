/*===================================================================*/
/*                                                                   */
/*                       Mapper 6 (FFE)  映射器 6（FFE）             */
/*                                                                   */
/*===================================================================*/

uint8_t Map6_IRQ_Enable;
uint32_t Map6_IRQ_Cnt;
uint8_t Map6_Chr_Ram[ 0x2000 * 4 ];

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 6  初始化映射器 6                              */
/*-------------------------------------------------------------------*/
void Map6_Init()
{
  int page_number;

  /* Initialize Mapper  初始化映射器 */
  MapperInit = Map6_Init;

  /* Write to Mapper  写入映射器 */
  MapperWrite = Map6_Write;

  /* Write to SRAM  写入 SRAM */
  MapperSram = Map0_Sram;

  /* Write to APU  写入 APU */
  MapperApu = Map6_Apu;

  /* Read from APU  从 APU 读取 */
  MapperReadApu = Map0_ReadApu;

  /* Callback at VSync  场同步回调 */
  MapperVSync = Map0_VSync;

  /* Callback at HSync  行同步回调 */
  MapperHSync = Map6_HSync;

  /* Callback at PPU  PPU 回调 */
  MapperPPU = Map0_PPU;

  /* Callback at Rendering Screen ( 1:BG, 0:Sprite )  渲染画面回调（1：背景，0：精灵） */
  MapperRenderScreen = Map0_RenderScreen;

  /* Set SRAM Banks  设置 SRAM 银行 */
  SRAMBANK = SRAM;

  /* Set ROM Banks  设置 ROM 银行 */
  ROMBANK0 = ROMPAGE( 0 );
  ROMBANK1 = ROMPAGE( 1 );
  ROMBANK2 = ROMPAGE( 14 );
  ROMBANK3 = ROMPAGE( 15 );

  /* Set PPU Banks  设置 PPU 银行 */
  if ( NesHeader.v_rom_size > 0 )
  {
    for ( page_number = 0; page_number < 8; ++page_number )
    {
      PPUBANK[ page_number ] = VROMPAGE( page_number );
    }
    InfoNES_SetupChr();
  }
  else
  {
    for ( page_number = 0; page_number < 8; ++page_number )
    {
      PPUBANK[ page_number ] = Map6_VROMPAGE( page_number );
    }
    InfoNES_SetupChr();
  }

  /* Set up wiring of the interrupt pin  设置中断引脚接线方式 */
  K6502_Set_Int_Wiring( 1, 1 ); 
}

/*-------------------------------------------------------------------*/
/*  Mapper 6 Write to APU Function  映射器 6：写 APU 函数            */
/*-------------------------------------------------------------------*/
void Map6_Apu( uint16_t address, uint8_t byte_data )
{
  switch ( address )
  {
    /* Name Table Mirroring  命名表镜像 */
    case 0x42fe:
      if ( byte_data & 0x10 )
      {
        InfoNES_Mirroring( 2 );
      } else {
        InfoNES_Mirroring( 3 );
      }
      break;

    case 0x42ff:
      if ( byte_data & 0x10 )
      {
        InfoNES_Mirroring( 0 );
      } else {
        InfoNES_Mirroring( 1 );
      }
      break;

    case 0x4501:
      Map6_IRQ_Enable = 0;
      break;

    case 0x4502:
      Map6_IRQ_Cnt = ( Map6_IRQ_Cnt & 0xff00 ) | (uint32_t)byte_data;
      break;

    case 0x4503:
      Map6_IRQ_Cnt = ( Map6_IRQ_Cnt & 0x00ff ) | ( (uint32_t)byte_data << 8 );
      Map6_IRQ_Enable = 1;
      break;
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 6 Write Function  映射器 6：写函数                        */
/*-------------------------------------------------------------------*/
void Map6_Write( uint16_t address, uint8_t byte_data )
{
  uint8_t prg_bank = ( byte_data & 0x3c ) >> 2;
  uint8_t chr_bank = byte_data & 0x03;

  /* Set ROM Banks  设置 ROM 银行 */
  prg_bank <<= 1;
  prg_bank %= ( NesHeader.rom_size << 1 );

  ROMBANK0 = ROMPAGE( prg_bank );
  ROMBANK1 = ROMPAGE( prg_bank + 1 );

  /* Set PPU Banks  设置 PPU 银行 */
  PPUBANK[ 0 ] = &Map6_Chr_Ram[ chr_bank * 0x2000 + 0 * 0x400 ];
  PPUBANK[ 1 ] = &Map6_Chr_Ram[ chr_bank * 0x2000 + 1 * 0x400 ];
  PPUBANK[ 2 ] = &Map6_Chr_Ram[ chr_bank * 0x2000 + 2 * 0x400 ];
  PPUBANK[ 3 ] = &Map6_Chr_Ram[ chr_bank * 0x2000 + 3 * 0x400 ];
  PPUBANK[ 4 ] = &Map6_Chr_Ram[ chr_bank * 0x2000 + 4 * 0x400 ];
  PPUBANK[ 5 ] = &Map6_Chr_Ram[ chr_bank * 0x2000 + 5 * 0x400 ];
  PPUBANK[ 6 ] = &Map6_Chr_Ram[ chr_bank * 0x2000 + 6 * 0x400 ];
  PPUBANK[ 7 ] = &Map6_Chr_Ram[ chr_bank * 0x2000 + 7 * 0x400 ];
  InfoNES_SetupChr();
}

/*-------------------------------------------------------------------*/
/*  Mapper 6 H-Sync Function  映射器 6：行同步函数                   */
/*-------------------------------------------------------------------*/
void Map6_HSync()
{
  if ( Map6_IRQ_Enable )
  {
    Map6_IRQ_Cnt += 133;
    if ( Map6_IRQ_Cnt >= 0xffff )
    {
      IRQ_REQ;
      Map6_IRQ_Cnt = 0;
    }
  }
}
