/*===================================================================*/
/*                                                                   */
/*                  Mapper 25 (Konami VRC4 type B)  映射器 25（Konami VRC4 type B） */
/*                                                                   */
/*===================================================================*/

uint8_t Map25_Bank_Selector;
uint8_t Map25_VBank[16];

uint8_t Map25_IRQ_Count;
uint8_t Map25_IRQ_State;
uint8_t Map25_IRQ_Latch;

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 25  初始化映射器 25                            */
/*-------------------------------------------------------------------*/
void Map25_Init()
{
  int page_number;

  /* Initialize Mapper  初始化映射器 */
  MapperInit = Map25_Init;

  /* Write to Mapper  写入映射器 */
  MapperWrite = Map25_Write;

  /* Write to SRAM  写入 SRAM */
  MapperSram = Map0_Sram;

  /* Write to APU  写入 APU */
  MapperApu = Map0_Apu;

  /* Read from APU  从 APU 读取 */
  MapperReadApu = Map0_ReadApu;

  /* Callback at VSync  场同步回调 */
  MapperVSync = Map0_VSync;

  /* Callback at HSync  行同步回调 */
  MapperHSync = Map25_HSync;

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

  /* Reset VBank Registers  重置 V Bank 寄存器 */
  for (page_number = 0; page_number < 16; page_number++)
    Map25_VBank[ page_number ] = 0x00;

  /* Set up wiring of the interrupt pin  设置中断引脚接线方式 */
  K6502_Set_Int_Wiring( 1, 1 ); 
}

/*-------------------------------------------------------------------*/
/*  Mapper 25 Write Function  映射器 25：写函数                      */
/*-------------------------------------------------------------------*/
void Map25_Write( uint16_t address, uint8_t byte_data )
{
  int bank_select;

  switch ( address )
  {
    case 0x8000:
      /* Set ROM Banks  设置 ROM 银行 */
      byte_data %= ( NesHeader.rom_size << 1 );
      if ( Map25_Bank_Selector ) 
      {
        ROMBANK2 = ROMPAGE( byte_data );
      } else {
        ROMBANK0 = ROMPAGE( byte_data );
      }
      break;

    case 0x9000:
      /* Name Table Mirroring  命名表镜像 */
      switch (byte_data & 0x03)
      {
        case 0:
          InfoNES_Mirroring( 1 );   /* Vertical  垂直 */
          break;
        case 1:
          InfoNES_Mirroring( 0 );   /* Horizontal  水平 */
          break;
        case 2:
          InfoNES_Mirroring( 2 );   /* One Screen 0x2000  单屏 0x2000 */
          break;
        case 3:
          InfoNES_Mirroring( 3 );   /* One Screen 0x2400  单屏 0x2400 */
          break;
      }
      break;

    case 0x9002:
      /* TODO: SaveRAM Toggle  TODO：切换存档 RAM 开关 */
      /* $8000 Switching Mode  $8000 切换模式 */
      Map25_Bank_Selector = byte_data & 0x02;
      break;

    case 0xa000:
      /* Set ROM Banks  设置 ROM 银行 */
      byte_data %= ( NesHeader.rom_size << 1 );
      ROMBANK1 = ROMPAGE( byte_data );
      break;
    
    default:
      /* Set PPU Banks  设置 PPU 银行 */
      switch ( address & 0xfffc)
      {
        case 0xb000:
          Map25_VBank[ 0 + ( address & 0x0003 ) ] = byte_data & 0x0f;
          bank_select = 0 + ( address & 0x0001 );
          Map25_Sync_Vrom( bank_select );
          break;
  
        case 0xc000:
          Map25_VBank[ 4 + ( address & 0x0003 ) ] = byte_data & 0x0f;
          bank_select = 2 + ( address & 0x0001 );
          Map25_Sync_Vrom( bank_select );
          break;

        case 0xd000:
          Map25_VBank[ 8 + ( address & 0x0003 ) ] = byte_data & 0x0f;
          bank_select = 4 + ( address & 0x0001 );
          Map25_Sync_Vrom( bank_select );
          break;

        case 0xe000:
          Map25_VBank[ 12 + ( address & 0x0003 ) ] = byte_data & 0x0f;
          bank_select = 6 + ( address & 0x0001 );
          Map25_Sync_Vrom( bank_select );
          break;

        case 0xf000:
          switch ( address & 0x0003 )
          {
            case 0:
            case 1:
              Map25_IRQ_Latch = byte_data;
              break;
            case 2:
              Map25_IRQ_State = ( byte_data & 0x01 ) ? Map25_IRQ_State : 0x00;
              Map25_IRQ_State = ( byte_data & 0x02 ) ? 0x01 : Map25_IRQ_State;
              Map25_IRQ_Count = Map25_IRQ_Latch;
              break;
            case 3:
              Map25_IRQ_State = ( Map25_IRQ_State << 1 ) | ( Map25_IRQ_State & 1 );
              break;
          }
          break;
      }
      break;
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 25 Sync VROM Function  映射器 25：同步 VROM 函数          */
/*-------------------------------------------------------------------*/
void Map25_Sync_Vrom( int bank_select )
{
  uint8_t table_value;
  
  table_value  = Map25_VBank[ ( bank_select << 1 ) - ( bank_select & 0x01 ) ];
  table_value |= Map25_VBank[ ( bank_select << 1 ) - ( bank_select & 0x01 ) + 2] << 4;
  table_value %= ( NesHeader.v_rom_size << 3 );
  PPUBANK[ bank_select ] = VROMPAGE( table_value );
  InfoNES_SetupChr();
}

/*-------------------------------------------------------------------*/
/*  Mapper 25 H-Sync Function  映射器 25：行同步函数                 */
/*-------------------------------------------------------------------*/
void Map25_HSync()
{
/*
 *  Callback at HSync  行同步回调
 *
 */
  if ( ( Map25_IRQ_State & 0x02 ) && ( ++Map25_IRQ_Count == 0 ) )
  {
    IRQ_REQ;
    Map25_IRQ_Count = Map25_IRQ_Latch;
  }
}
