/*===================================================================*/
/*                                                                   */
/*                     Mapper 73 (Konami VRC 3)  映射器 73（Konami VRC 3） */
/*                                                                   */
/*===================================================================*/

uint8_t  Map73_IRQ_Enable;
uint32_t Map73_IRQ_Cnt;

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 73  初始化映射器 73                            */
/*-------------------------------------------------------------------*/
void Map73_Init()
{
  /* Initialize Mapper  初始化映射器 */
  MapperInit = Map73_Init;

  /* Write to Mapper  写入映射器 */
  MapperWrite = Map73_Write;

  /* Write to SRAM  写入 SRAM */
  MapperSram = Map0_Sram;

  /* Write to APU  写入 APU */
  MapperApu = Map0_Apu;

  /* Read from APU  从 APU 读取 */
  MapperReadApu = Map0_ReadApu;

  /* Callback at VSync  场同步回调 */
  MapperVSync = Map0_VSync;

  /* Callback at HSync  行同步回调 */
  MapperHSync = Map73_HSync;

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

  /* Initialize IRQ Registers  初始化 IRQ 寄存器 */
  Map73_IRQ_Enable = 0;
  Map73_IRQ_Cnt = 0;  

  /* Set up wiring of the interrupt pin  设置中断引脚接线方式 */
  K6502_Set_Int_Wiring( 1, 1 ); 
}

/*-------------------------------------------------------------------*/
/*  Mapper 73 Write Function  映射器 73：写函数                      */
/*-------------------------------------------------------------------*/
void Map73_Write( uint16_t address, uint8_t byte_data )
{
  switch ( address )
  {
    case 0x8000:
      Map73_IRQ_Cnt = ( Map73_IRQ_Cnt & 0xfff0 ) | ( byte_data & 0x0f );
      break;

    case 0x9000:
      Map73_IRQ_Cnt = ( Map73_IRQ_Cnt & 0xff0f ) | ( ( byte_data & 0x0f ) << 4 );
      break;

    case 0xa000:
      Map73_IRQ_Cnt = ( Map73_IRQ_Cnt & 0xf0ff ) | ( ( byte_data & 0x0f ) << 8 );
      break;

    case 0xb000:
      Map73_IRQ_Cnt = ( Map73_IRQ_Cnt & 0x0fff ) | ( ( byte_data & 0x0f ) << 12 );
      break;

    case 0xc000:
      Map73_IRQ_Enable = byte_data;
      break;

    /* Set ROM Banks  设置 ROM 银行 */
    case 0xf000:
      byte_data <<= 1;
      byte_data %= ( NesHeader.rom_size << 1 );
      ROMBANK0 = ROMPAGE( byte_data );
      ROMBANK1 = ROMPAGE( byte_data + 1 );
      break;
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 73 H-Sync Function  映射器 73：行同步函数                 */
/*-------------------------------------------------------------------*/
void Map73_HSync()
{
/*
 *  Callback at HSync  行同步回调
 *
 */
#if 1
  if ( Map73_IRQ_Enable & 0x02 )
  {
    if ( ( Map73_IRQ_Cnt += STEP_PER_SCANLINE ) > 0xffff )
    {
      Map73_IRQ_Cnt &= 0xffff;
      IRQ_REQ;
      Map73_IRQ_Enable = 0;
    }
  }
#else
  if ( Map73_IRQ_Enable )
  {
    if ( Map73_IRQ_Cnt > 0xffff - 114 )
    {
      IRQ_REQ;
      Map73_IRQ_Enable = 0;
    } else {
      Map73_IRQ_Cnt += 114;
    }
  }
#endif
}
