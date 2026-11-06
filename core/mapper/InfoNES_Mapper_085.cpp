/*===================================================================*/
/*                                                                   */
/*                    Mapper 85 (Konami VRC7)  映射器 85（Konami VRC7） */
/*                                                                   */
/*===================================================================*/

uint8_t Map85_Chr_Ram[ 0x100 * 0x400 ];
uint8_t Map85_Regs[ 1 ];
uint8_t Map85_IRQ_Enable;
uint8_t Map85_IRQ_Cnt;
uint8_t Map85_IRQ_Latch;

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 85  初始化映射器 85                            */
/*-------------------------------------------------------------------*/
void Map85_Init()
{
  /* Initialize Mapper  初始化映射器 */
  MapperInit = Map85_Init;

  /* Write to Mapper  写入映射器 */
  MapperWrite = Map85_Write;

  /* Write to SRAM  写入 SRAM */
  MapperSram = Map0_Sram;

  /* Write to APU  写入 APU */
  MapperApu = Map0_Apu;

  /* Read from APU  从 APU 读取 */
  MapperReadApu = Map0_ReadApu;

  /* Callback at VSync  场同步回调 */
  MapperVSync = Map0_VSync;

  /* Callback at HSync  行同步回调 */
  MapperHSync = Map85_HSync;

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
  PPUBANK[ 0 ] = Map85_VROMPAGE( 0 );
  PPUBANK[ 1 ] = Map85_VROMPAGE( 0 );
  PPUBANK[ 2 ] = Map85_VROMPAGE( 0 );
  PPUBANK[ 3 ] = Map85_VROMPAGE( 0 );
  PPUBANK[ 4 ] = Map85_VROMPAGE( 0 );
  PPUBANK[ 5 ] = Map85_VROMPAGE( 0 );
  PPUBANK[ 6 ] = Map85_VROMPAGE( 0 );
  PPUBANK[ 7 ] = Map85_VROMPAGE( 0 );
  InfoNES_SetupChr();

  /* Initialize State Registers  初始化状态寄存器 */
  Map85_Regs[ 0 ] = 0;
  Map85_IRQ_Enable = 0;
  Map85_IRQ_Cnt = 0;
  Map85_IRQ_Latch = 0;

  /* Set up wiring of the interrupt pin  设置中断引脚接线方式 */
  K6502_Set_Int_Wiring( 1, 1 ); 
}

/*-------------------------------------------------------------------*/
/*  Mapper 85 Write Function  映射器 85：写函数                      */
/*-------------------------------------------------------------------*/
void Map85_Write( uint16_t address, uint8_t byte_data )
{
  switch( address & 0xf030 )
  {
    case 0x8000:
      byte_data %= ( NesHeader.rom_size << 1 );
      ROMBANK0 = ROMPAGE( byte_data );
      break;

    case 0x8010:
      byte_data %= ( NesHeader.rom_size << 1 );
      ROMBANK1 = ROMPAGE( byte_data );
      break;

    case 0x9000:
      byte_data %= ( NesHeader.rom_size << 1 );
      ROMBANK2 = ROMPAGE( byte_data );
      break;

    case 0x9010:
    case 0x9030:
      /* Extra Sound  额外声音 */

    case 0xa000:
      PPUBANK[ 0 ] = Map85_VROMPAGE( byte_data );
      InfoNES_SetupChr();
      break;

    case 0xa010:
      PPUBANK[ 1 ] = Map85_VROMPAGE( byte_data );
      InfoNES_SetupChr();
      break;

    case 0xb000:
      PPUBANK[ 2 ] = Map85_VROMPAGE( byte_data );
      InfoNES_SetupChr();
      break;

    case 0xb010:
      PPUBANK[ 3 ] = Map85_VROMPAGE( byte_data );
      InfoNES_SetupChr();
      break;

    case 0xc000:
      PPUBANK[ 4 ] = Map85_VROMPAGE( byte_data );
      InfoNES_SetupChr();
      break;

    case 0xc010:
      PPUBANK[ 5 ] = Map85_VROMPAGE( byte_data );
      InfoNES_SetupChr();
      break;

    case 0xd000:
      PPUBANK[ 6 ] = Map85_VROMPAGE( byte_data );
      InfoNES_SetupChr();
      break;

    case 0xd010:
      PPUBANK[ 7 ] = Map85_VROMPAGE( byte_data );
      InfoNES_SetupChr();
      break;

    /* Name Table Mirroring  命名表镜像 */
    case 0xe000:
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

    case 0xe010:
      Map85_IRQ_Latch = 0xff - byte_data;
      break;

    case 0xf000:
      Map85_Regs[ 0 ] = byte_data & 0x01;
      Map85_IRQ_Enable = ( byte_data & 0x02 ) >> 1;
      Map85_IRQ_Cnt = Map85_IRQ_Latch;
      break;

    case 0xf010:
      Map85_IRQ_Enable = Map85_Regs[ 0 ];
      Map85_IRQ_Cnt = Map85_IRQ_Latch;
      break;
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 85 H-Sync Function  映射器 85：行同步函数                 */
/*-------------------------------------------------------------------*/
void Map85_HSync()
{
  if ( Map85_IRQ_Enable )
  {
    if ( Map85_IRQ_Cnt == 0 )
    {
      IRQ_REQ;
      Map85_IRQ_Enable = 0;
    } else {
      Map85_IRQ_Cnt--;
    }
  }
}
