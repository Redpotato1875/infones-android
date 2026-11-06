/*===================================================================*/
/*                                                                   */
/*                     Mapper 42 (Pirates)  映射器 42（Pirates）     */
/*                                                                   */
/*===================================================================*/

uint8_t Map42_IRQ_Cnt;
uint8_t Map42_IRQ_Enable;

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 42  初始化映射器 42                            */
/*-------------------------------------------------------------------*/
void Map42_Init()
{
  /* Initialize Mapper  初始化映射器 */
  MapperInit = Map42_Init;

  /* Write to Mapper  写入映射器 */
  MapperWrite = Map42_Write;

  /* Write to SRAM  写入 SRAM */
  MapperSram = Map0_Sram;

  /* Write to APU  写入 APU */
  MapperApu = Map0_Apu;

  /* Read from APU  从 APU 读取 */
  MapperReadApu = Map0_ReadApu;

  /* Callback at VSync  场同步回调 */
  MapperVSync = Map0_VSync;

  /* Callback at HSync  行同步回调 */
  MapperHSync = Map42_HSync;

  /* Callback at PPU  PPU 回调 */
  MapperPPU = Map0_PPU;

  /* Callback at Rendering Screen ( 1:BG, 0:Sprite )  渲染画面回调（1：背景，0：精灵） */
  MapperRenderScreen = Map0_RenderScreen;

  /* Set SRAM Banks  设置 SRAM 银行 */
  SRAMBANK = ROMPAGE( 0 );

  /* Set ROM Banks  设置 ROM 银行 */
  ROMBANK0 = ROMLASTPAGE( 3 );
  ROMBANK1 = ROMLASTPAGE( 2 );
  ROMBANK2 = ROMLASTPAGE( 1 );
  ROMBANK3 = ROMLASTPAGE( 0 );

  /* Set PPU Banks  设置 PPU 银行 */
  if ( NesHeader.v_rom_size > 0 )
  {
    for ( int page_number = 0; page_number < 8; ++page_number )
      PPUBANK[ page_number ] = VROMPAGE( page_number );
    InfoNES_SetupChr();
  }

  /* Set up wiring of the interrupt pin  设置中断引脚接线方式 */
  K6502_Set_Int_Wiring( 1, 1 ); 
}

/*-------------------------------------------------------------------*/
/*  Mapper 42 Write Function  映射器 42：写函数                      */
/*-------------------------------------------------------------------*/
void Map42_Write( uint16_t address, uint8_t byte_data )
{
  switch ( address & 0xe003 )
  {
    /* Set ROM Banks  设置 ROM 银行 */
    case 0xe000:
      SRAMBANK = ROMPAGE( ( byte_data & 0x0f ) % ( NesHeader.rom_size << 1 ) );
      break;

    case 0xe001:
      if ( byte_data & 0x08 )
      {
        InfoNES_Mirroring( 0 );
      } else {
        InfoNES_Mirroring( 1 );
      }
      break;

    case 0xe002:
      if ( byte_data & 0x02 )
      {
        Map42_IRQ_Enable = 1;
      } else {
        Map42_IRQ_Enable = 0;
        Map42_IRQ_Cnt = 0;
      }
      break;
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 42 H-Sync Function  映射器 42：行同步函数                 */
/*-------------------------------------------------------------------*/
void Map42_HSync()
{
/*
 *  Callback at HSync  行同步回调
 *
 */
  if ( Map42_IRQ_Enable )
  {
    if ( Map42_IRQ_Cnt < 215 )
    {
      Map42_IRQ_Cnt++;
    }
    if ( Map42_IRQ_Cnt == 215 )
    {
      IRQ_REQ;
      Map42_IRQ_Enable = 0;
    }
  }
}
