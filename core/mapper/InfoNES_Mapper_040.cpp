/*===================================================================*/
/*                                                                   */
/*                       Mapper 40 (SMB2J)  映射器 40（SMB2J）       */
/*                                                                   */
/*===================================================================*/

uint8_t  Map40_IRQ_Enable;
uint32_t Map40_Line_To_IRQ;

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 40  初始化映射器 40                            */
/*-------------------------------------------------------------------*/
void Map40_Init()
{
  /* Initialize Mapper  初始化映射器 */
  MapperInit = Map40_Init;

  /* Write to Mapper  写入映射器 */
  MapperWrite = Map40_Write;

  /* Write to SRAM  写入 SRAM */
  MapperSram = Map0_Sram;

  /* Write to APU  写入 APU */
  MapperApu = Map0_Apu;

  /* Read from APU  从 APU 读取 */
  MapperReadApu = Map0_ReadApu;

  /* Callback at VSync  场同步回调 */
  MapperVSync = Map0_VSync;

  /* Callback at HSync  行同步回调 */
  MapperHSync = Map40_HSync;

  /* Callback at PPU  PPU 回调 */
  MapperPPU = Map0_PPU;

  /* Callback at Rendering Screen ( 1:BG, 0:Sprite )  渲染画面回调（1：背景，0：精灵） */
  MapperRenderScreen = Map0_RenderScreen;

  /* Set SRAM Banks  设置 SRAM 银行 */
  SRAMBANK = ROMPAGE( 6 );

  /* Initialize IRQ Registers  初始化 IRQ 寄存器 */
  Map40_IRQ_Enable = 0;
  Map40_Line_To_IRQ = 0;

  /* Set ROM Banks  设置 ROM 银行 */
  ROMBANK0 = ROMPAGE( 4 );
  ROMBANK1 = ROMPAGE( 5 );
  ROMBANK2 = ROMPAGE( 0 );
  ROMBANK3 = ROMPAGE( 7 );

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
/*  Mapper 40 Write Function  映射器 40：写函数                      */
/*-------------------------------------------------------------------*/
void Map40_Write( uint16_t address, uint8_t byte_data )
{
  switch ( address & 0xe000 )
  {
    case 0x8000:
      Map40_IRQ_Enable = 0;
      break;

    case 0xa000:
      Map40_IRQ_Enable = 1;
      Map40_Line_To_IRQ = 37;
      break;

    case 0xc000:
      break;

    case 0xe000:
      /* Set ROM Banks  设置 ROM 银行 */
      ROMBANK2 = ROMPAGE ( ( byte_data & 0x07 ) % ( NesHeader.rom_size << 1 ) );
      break;
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 40 H-Sync Function  映射器 40：行同步函数                 */
/*-------------------------------------------------------------------*/
void Map40_HSync()
{
/*
 *  Callback at HSync  行同步回调
 *
 */
  if ( Map40_IRQ_Enable )
  {
    if ( ( --Map40_Line_To_IRQ ) <= 0 )
    {
      IRQ_REQ;
    }
  }
}

/* End of InfoNES_Mapper_40.cpp  InfoNES_Mapper_40.cpp 结束 */
