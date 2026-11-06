/*===================================================================*/
/*                                                                   */
/*                   Mapper 16 (Bandai Mapper)  映射器 16（Bandai Mapper） */
/*                                                                   */
/*===================================================================*/

uint8_t  Map16_Regs[3];

uint8_t  Map16_IRQ_Enable;
uint32_t Map16_IRQ_Cnt;
uint32_t Map16_IRQ_Latch;

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 16  初始化映射器 16                            */
/*-------------------------------------------------------------------*/
void Map16_Init()
{
  /* Initialize Mapper  初始化映射器 */
  MapperInit = Map16_Init;

  /* Write to Mapper  写入映射器 */
  MapperWrite = Map16_Write;

  /* Write to SRAM  写入 SRAM */
  MapperSram = Map16_Write;

  /* Write to APU  写入 APU */
  MapperApu = Map0_Apu;

  /* Read from APU  从 APU 读取 */
  MapperReadApu = Map0_ReadApu;

  /* Callback at VSync  场同步回调 */
  MapperVSync = Map0_VSync;

  /* Callback at HSync  行同步回调 */
  MapperHSync = Map16_HSync;

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

  /* Initialize State Flag  初始化状态标志 */
  Map16_Regs[ 0 ] = 0;
  Map16_Regs[ 1 ] = 0;
  Map16_Regs[ 2 ] = 0;

  Map16_IRQ_Enable = 0;
  Map16_IRQ_Cnt = 0;
  Map16_IRQ_Latch = 0;

  /* Set up wiring of the interrupt pin  设置中断引脚接线方式 */
  K6502_Set_Int_Wiring( 1, 1 ); 
}

/*-------------------------------------------------------------------*/
/*  Mapper 16 Write Function  映射器 16：写函数                      */
/*-------------------------------------------------------------------*/
void Map16_Write( uint16_t address, uint8_t byte_data )
{
  switch ( address & 0x000f )
  {
    case 0x0000:
      byte_data %= ( NesHeader.v_rom_size << 3 );
      PPUBANK[ 0 ] = VROMPAGE( byte_data );
      InfoNES_SetupChr();
      break;

    case 0x0001:
      byte_data %= ( NesHeader.v_rom_size << 3 );
      PPUBANK[ 1 ] = VROMPAGE( byte_data );
      InfoNES_SetupChr();
      break;

    case 0x0002:
      byte_data %= ( NesHeader.v_rom_size << 3 );
      PPUBANK[ 2 ] = VROMPAGE( byte_data );
      InfoNES_SetupChr();
      break;

    case 0x0003:
      byte_data %= ( NesHeader.v_rom_size << 3 );
      PPUBANK[ 3 ] = VROMPAGE( byte_data );
      InfoNES_SetupChr();
      break;

    case 0x0004:
      byte_data %= ( NesHeader.v_rom_size << 3 );
      PPUBANK[ 4 ] = VROMPAGE( byte_data );
      InfoNES_SetupChr();
      break;

    case 0x0005:
      byte_data %= ( NesHeader.v_rom_size << 3 );
      PPUBANK[ 5 ] = VROMPAGE( byte_data );
      InfoNES_SetupChr();
      break;

    case 0x0006:
      byte_data %= ( NesHeader.v_rom_size << 3 );
      PPUBANK[ 6 ] = VROMPAGE( byte_data );
      InfoNES_SetupChr();
      break;

    case 0x0007:
      byte_data %= ( NesHeader.v_rom_size << 3 );
      PPUBANK[ 7 ] = VROMPAGE( byte_data );
      InfoNES_SetupChr();
      break;

    case 0x0008:
      byte_data <<= 1;
      byte_data %= ( NesHeader.rom_size << 1 );
      ROMBANK0 = ROMPAGE( byte_data );
      ROMBANK1 = ROMPAGE( byte_data + 1 );
      break;

    case 0x0009:
      switch ( byte_data & 0x03 )
      {
        case 0x00:
          InfoNES_Mirroring( 1 );
          break;

        case 0x01:
          InfoNES_Mirroring( 0 );
          break;    

        case 0x02:
          InfoNES_Mirroring( 3 );
          break;

        case 0x03:
          InfoNES_Mirroring( 2 );
          break; 
      }
      break;

      case 0x000a:
        Map16_IRQ_Enable = byte_data & 0x01;
        Map16_IRQ_Cnt = Map16_IRQ_Latch;
        break;

      case 0x000b:
        Map16_IRQ_Latch = ( Map16_IRQ_Latch & 0xff00 ) | byte_data;
        break;

      case 0x000c:
        Map16_IRQ_Latch = ( (uint32_t)byte_data << 8 ) | ( Map16_IRQ_Latch & 0x00ff );
        break;

      case 0x000d:
        /* Write Protect  写保护 */
        break;
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 16 H-Sync Function  映射器 16：行同步函数                 */
/*-------------------------------------------------------------------*/
void Map16_HSync()
{
  if ( Map16_IRQ_Enable )
  {
    /* Normal IRQ  普通 IRQ */
    if ( Map16_IRQ_Cnt <= 114 )
    {
      IRQ_REQ;
      Map16_IRQ_Cnt = 0;
      Map16_IRQ_Enable = 0;
    } else {
      Map16_IRQ_Cnt -= 114;
    }
  }
}
