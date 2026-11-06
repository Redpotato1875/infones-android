/*===================================================================*/
/*                                                                   */
/*                       Mapper 32 (Irem G-101)  映射器 32（Irem G-101） */
/*                                                                   */
/*===================================================================*/

uint8_t Map32_Saved;

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 32  初始化映射器 32                            */
/*-------------------------------------------------------------------*/
void Map32_Init()
{
  /* Initialize Mapper  初始化映射器 */
  MapperInit = Map32_Init;

  /* Write to Mapper  写入映射器 */
  MapperWrite = Map32_Write;

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

  /* Initialize state flag  初始化状态标志 */
  Map32_Saved = 0x00;

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

  /* Set up wiring of the interrupt pin  设置中断引脚接线方式 */
  K6502_Set_Int_Wiring( 1, 1 );
}

/*-------------------------------------------------------------------*/
/*  Mapper 32 Write Function  映射器 32：写函数                      */
/*-------------------------------------------------------------------*/
void Map32_Write( uint16_t address, uint8_t byte_data )
{
  switch ( address & 0xf000 )
  {
    case 0x8000:
      /* Set ROM Banks  设置 ROM 银行 */
      byte_data %= ( NesHeader.rom_size << 1 );

      if ( Map32_Saved & 0x02 ) 
      {
        ROMBANK2 = ROMPAGE( byte_data );
      } else {
        ROMBANK0 = ROMPAGE( byte_data );
      }
      break;
      
    case 0x9000:
      Map32_Saved = byte_data;
      
      // Name Table Mirroring  命名表镜像
      InfoNES_Mirroring( byte_data & 0x01 );
      break;

    case 0xa000:
      /* Set ROM Banks  设置 ROM 银行 */
      byte_data %= ( NesHeader.rom_size << 1 );
      ROMBANK1 = ROMPAGE( byte_data );
      break;

    case 0xb000:
      /* Set PPU Banks  设置 PPU 银行 */
      byte_data %= ( NesHeader.v_rom_size << 3 );
      PPUBANK[ address & 0x0007 ] = VROMPAGE( byte_data );
      InfoNES_SetupChr();
      break;

    default:
      break;
  }
}
