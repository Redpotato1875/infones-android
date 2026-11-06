/*===================================================================*/
/*                                                                   */
/*                  Mapper 69 (Sunsoft FME-7)  映射器 69（Sunsoft FME-7） */
/*                                                                   */
/*===================================================================*/

uint8_t  Map69_IRQ_Enable;
uint32_t Map69_IRQ_Cnt;
uint8_t  Map69_Regs[ 1 ];

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 69  初始化映射器 69                            */
/*-------------------------------------------------------------------*/
void Map69_Init()
{
  /* Initialize Mapper  初始化映射器 */
  MapperInit = Map69_Init;

  /* Write to Mapper  写入映射器 */
  MapperWrite = Map69_Write;

  /* Write to SRAM  写入 SRAM */
  MapperSram = Map0_Sram;

  /* Write to APU  写入 APU */
  MapperApu = Map0_Apu;

  /* Read from APU  从 APU 读取 */
  MapperReadApu = Map0_ReadApu;

  /* Callback at VSync  场同步回调 */
  MapperVSync = Map0_VSync;

  /* Callback at HSync  行同步回调 */
  MapperHSync = Map69_HSync;

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

  /* Initialize IRQ Reg  初始化 IRQ 寄存器 */
  Map69_IRQ_Enable = 0;
  Map69_IRQ_Cnt    = 0;

  /* Set up wiring of the interrupt pin  设置中断引脚接线方式 */
  K6502_Set_Int_Wiring( 1, 1 ); 
}

/*-------------------------------------------------------------------*/
/*  Mapper 69 Write Function  映射器 69：写函数                      */
/*-------------------------------------------------------------------*/
void Map69_Write( uint16_t address, uint8_t byte_data )
{
  switch ( address )
  {
    case 0x8000:
      Map69_Regs[ 0 ] = byte_data & 0x0f;
      break;

    case 0xA000:
      switch ( Map69_Regs[ 0 ] )
      {
        /* Set PPU Banks  设置 PPU 银行 */
        case 0x00:
        case 0x01:
        case 0x02:
        case 0x03:
        case 0x04:
        case 0x05:
        case 0x06:
        case 0x07:
          byte_data %= ( NesHeader.v_rom_size << 3 );
          PPUBANK[ Map69_Regs[ 0 ] ] = VROMPAGE( byte_data );
          InfoNES_SetupChr();
          break;

        /* Set ROM Banks  设置 ROM 银行 */
#if 0
        case 0x08:
          if ( !( byte_data & 0x40 ) )
          {
            byte_data %= ( NesHeader.rom_size << 1 );
            SRAMBANK = ROMPAGE( byte_data );
          }
          break;
#endif

        case 0x09:
          byte_data %= ( NesHeader.rom_size << 1 );
          ROMBANK0 = ROMPAGE( byte_data );
          break;

        case 0x0a:
          byte_data %= ( NesHeader.rom_size << 1 );
          ROMBANK1 = ROMPAGE( byte_data );
          break;

        case 0x0b:
          byte_data %= ( NesHeader.rom_size << 1 );
          ROMBANK2 = ROMPAGE( byte_data );
          break;

        /* Name Table Mirroring  命名表镜像 */
        case 0x0c:  
          switch ( byte_data & 0x03 )
          {
            case 0:
              InfoNES_Mirroring( 1 );   /* Vertical  垂直 */
              break;
            case 1:
              InfoNES_Mirroring( 0 );   /* Horizontal  水平 */
              break;
            case 2:
              InfoNES_Mirroring( 3 );   /* One Screen 0x2400  单屏 0x2400 */
              break;
            case 3:
              InfoNES_Mirroring( 2 );   /* One Screen 0x2000  单屏 0x2000 */
              break;
          }
          break;

        case 0x0d:
          Map69_IRQ_Enable = byte_data;
          break;

        case 0x0e:
          Map69_IRQ_Cnt = ( Map69_IRQ_Cnt & 0xff00) | (uint32_t)byte_data;
          break;

        case 0x0f:
          Map69_IRQ_Cnt = ( Map69_IRQ_Cnt & 0x00ff) | ( (uint32_t)byte_data << 8 );
          break;
      }
      break;
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 69 H-Sync Function  映射器 69：行同步函数                 */
/*-------------------------------------------------------------------*/
void Map69_HSync()
{
/*
 *  Callback at HSync  行同步回调
 *
 */
  if ( Map69_IRQ_Enable )
  {
    if ( Map69_IRQ_Cnt <= 113 )
    {
      IRQ_REQ;
      Map69_IRQ_Cnt = 0;
    } else {
      Map69_IRQ_Cnt -= 113;
    }
  }
}
