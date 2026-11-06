/*===================================================================*/
/*                                                                   */
/*                  Mapper 26 (Konami VRC 6V)  映射器 26（Konami VRC 6V） */
/*                                                                   */
/*===================================================================*/

uint8_t Map26_IRQ_Enable;
uint8_t Map26_IRQ_Cnt;
uint8_t Map26_IRQ_Latch;

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 26  初始化映射器 26                            */
/*-------------------------------------------------------------------*/
void Map26_Init()
{
  /* Initialize Mapper  初始化映射器 */
  MapperInit = Map26_Init;

  /* Write to Mapper  写入映射器 */
  MapperWrite = Map26_Write;

  /* Write to SRAM  写入 SRAM */
  MapperSram = Map0_Sram;

  /* Write to APU  写入 APU */
  MapperApu = Map0_Apu;

  /* Read from APU  从 APU 读取 */
  MapperReadApu = Map0_ReadApu;

  /* Callback at VSync  场同步回调 */
  MapperVSync = Map0_VSync;

  /* Callback at HSync  行同步回调 */
  MapperHSync = Map26_HSync;

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
  Map26_IRQ_Enable = 0;
  Map26_IRQ_Cnt = 0;

  /* Set up wiring of the interrupt pin  设置中断引脚接线方式 */
  K6502_Set_Int_Wiring( 1, 1 ); 
}

/*-------------------------------------------------------------------*/
/*  Mapper 26 Write Function  映射器 26：写函数                      */
/*-------------------------------------------------------------------*/
void Map26_Write( uint16_t address, uint8_t byte_data )
{
  switch ( address )
  {
    /* Set ROM Banks  设置 ROM 银行 */
    case 0x8000:
      byte_data <<= 1;      
      byte_data %= ( NesHeader.rom_size << 1 );
      
      ROMBANK0 = ROMPAGE( byte_data + 0 );
      ROMBANK1 = ROMPAGE( byte_data + 1 );
      break;

    /* Name Table Mirroring  命名表镜像 */
    case 0xb003:  
      switch ( byte_data & 0x7f )
      {
        case 0x08:
        case 0x2c:
          InfoNES_Mirroring( 2 );   /* One Screen 0x2400  单屏 0x2400 */
          break;
        case 0x20:
          InfoNES_Mirroring( 1 );   /* Vertical  垂直 */
          break;
        case 0x24:
          InfoNES_Mirroring( 0 );   /* Horizontal  水平 */
          break;
        case 0x28:
          InfoNES_Mirroring( 3 );   /* One Screen 0x2000  单屏 0x2000 */
          break;
      }
      break;

    /* Set ROM Banks  设置 ROM 银行 */
    case 0xc000:
      byte_data %= ( NesHeader.rom_size << 1 );
      ROMBANK2 = ROMPAGE( byte_data );      
      InfoNES_SetupChr();
      break;

    /* Set PPU Bank  设置 PPU 银行 */
    case 0xd000:
      byte_data %= ( NesHeader.v_rom_size << 3 );
      PPUBANK[ 0 ] = VROMPAGE( byte_data );
      InfoNES_SetupChr();
      break;

    case 0xd001:
      byte_data %= ( NesHeader.v_rom_size << 3 );
      PPUBANK[ 2 ] = VROMPAGE( byte_data );
      InfoNES_SetupChr();
      break;

    case 0xd002:
      byte_data %= ( NesHeader.v_rom_size << 3 );
      PPUBANK[ 1 ] = VROMPAGE( byte_data );
      InfoNES_SetupChr();
      break;

    case 0xd003:
      byte_data %= ( NesHeader.v_rom_size << 3 );
      PPUBANK[ 3 ] = VROMPAGE( byte_data );
      InfoNES_SetupChr();
      break;

    case 0xe000:
      byte_data %= ( NesHeader.v_rom_size << 3 );
      PPUBANK[ 4 ] = VROMPAGE( byte_data );
      InfoNES_SetupChr();
      break;

    case 0xe001:
      byte_data %= ( NesHeader.v_rom_size << 3 );
      PPUBANK[ 6 ] = VROMPAGE( byte_data );
      InfoNES_SetupChr();
      break;

    case 0xe002:
      byte_data %= ( NesHeader.v_rom_size << 3 );
      PPUBANK[ 5 ] = VROMPAGE( byte_data );
      InfoNES_SetupChr();
      break;

    case 0xe003:
      byte_data %= ( NesHeader.v_rom_size << 3 );
      PPUBANK[ 7 ] = VROMPAGE( byte_data );
      InfoNES_SetupChr();
      break;

    /* Set IRQ Registers  设置 IRQ 寄存器 */
    case 0xf000:
      Map26_IRQ_Latch = byte_data;
      break;

    case 0xf001:
      Map26_IRQ_Enable = byte_data & 0x01;
      break;

    case 0xf002:
      Map26_IRQ_Enable = byte_data & 0x03;

      if ( Map26_IRQ_Enable & 0x02 )
      {
        Map26_IRQ_Cnt = Map26_IRQ_Latch;
      }
      break;
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 26 H-Sync Function  映射器 26：行同步函数                 */
/*-------------------------------------------------------------------*/
void Map26_HSync()
{
/*
 *  Callback at HSync  行同步回调
 *
 */
  if ( Map26_IRQ_Enable & 0x03 )
  {
    if ( Map26_IRQ_Cnt >= 0xfe )
    {
      IRQ_REQ;
      Map26_IRQ_Cnt = Map26_IRQ_Latch;
      Map26_IRQ_Enable = 0;
    } else {
      Map26_IRQ_Cnt++;
    }
  }
}
