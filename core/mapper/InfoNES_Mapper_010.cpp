/*===================================================================*/
/*                                                                   */
/*                     Mapper 10 (MMC4)  映射器 10（MMC4）           */
/*                                                                   */
/*===================================================================*/

struct Map10_Latch 
{
  uint8_t lo_bank;
  uint8_t hi_bank;
  uint8_t state;
};

struct Map10_Latch latch3;    // Latch Selector #1  锁存选择器 1
struct Map10_Latch latch4;    // Latch Selector #2  锁存选择器 2

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 10  初始化映射器 10                            */
/*-------------------------------------------------------------------*/
void Map10_Init()
{
  int page_number;

  /* Initialize Mapper  初始化映射器 */
  MapperInit = Map10_Init;

  /* Write to Mapper  写入映射器 */
  MapperWrite = Map10_Write;

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
  MapperPPU = Map10_PPU;

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
    for ( page_number = 0; page_number < 8; ++page_number )
      PPUBANK[ page_number ] = VROMPAGE( page_number );
    InfoNES_SetupChr();
  }

  /* Init Latch Selector  初始化锁存选择器 */
  latch3.state = 0xfe;
  latch3.lo_bank = 0;
  latch3.hi_bank = 0;
  latch4.state = 0xfe;
  latch4.lo_bank = 0;
  latch4.hi_bank = 0;

  /* Set up wiring of the interrupt pin  设置中断引脚接线方式 */
  K6502_Set_Int_Wiring( 1, 1 );
}

/*-------------------------------------------------------------------*/
/*  Mapper 10 Write Function  映射器 10：写函数                      */
/*-------------------------------------------------------------------*/
void Map10_Write( uint16_t address, uint8_t byte_data )
{
  uint16_t map_addr;

  map_addr = address & 0xf000;
  switch ( map_addr )
  {
    case 0xa000:
      /* Set ROM Banks  设置 ROM 银行 */
      byte_data %= NesHeader.rom_size;
      byte_data <<= 1;
      ROMBANK0 = ROMPAGE( byte_data );
      ROMBANK1 = ROMPAGE( byte_data + 1 );
      break;

    case 0xb000:
      /* Number of 4K Banks to Number of 1K Banks  把 4K 银行数换算为 1K 银行数 */
      byte_data %= ( NesHeader.v_rom_size << 1 );
      byte_data <<= 2;

      /* Latch Control  锁存控制 */
      latch3.lo_bank = byte_data;

      if (0xfd == latch3.state)
      {
        /* Set PPU Banks  设置 PPU 银行 */
        PPUBANK[ 0 ] = VROMPAGE( byte_data );
        PPUBANK[ 1 ] = VROMPAGE( byte_data + 1 );
        PPUBANK[ 2 ] = VROMPAGE( byte_data + 2 );
        PPUBANK[ 3 ] = VROMPAGE( byte_data + 3 );     
        InfoNES_SetupChr();
      }
      break;

    case 0xc000:
      /* Number of 4K Banks to Number of 1K Banks  把 4K 银行数换算为 1K 银行数 */
      byte_data %= ( NesHeader.v_rom_size << 1 );
      byte_data <<= 2;

      /* Latch Control  锁存控制 */
      latch3.hi_bank = byte_data;

      if (0xfe == latch3.state)
      {
        /* Set PPU Banks  设置 PPU 银行 */
        PPUBANK[ 0 ] = VROMPAGE( byte_data );
        PPUBANK[ 1 ] = VROMPAGE( byte_data + 1 );
        PPUBANK[ 2 ] = VROMPAGE( byte_data + 2 );
        PPUBANK[ 3 ] = VROMPAGE( byte_data + 3 );     
        InfoNES_SetupChr();
      }
      break;

    case 0xd000:
      /* Number of 4K Banks to Number of 1K Banks  把 4K 银行数换算为 1K 银行数 */
      byte_data %= ( NesHeader.v_rom_size << 1 );
      byte_data <<= 2;

      /* Latch Control  锁存控制 */
      latch4.lo_bank = byte_data;

      if (0xfd == latch4.state)
      {
        /* Set PPU Banks  设置 PPU 银行 */
        PPUBANK[ 4 ] = VROMPAGE( byte_data );
        PPUBANK[ 5 ] = VROMPAGE( byte_data + 1 );
        PPUBANK[ 6 ] = VROMPAGE( byte_data + 2 );
        PPUBANK[ 7 ] = VROMPAGE( byte_data + 3 );    
        InfoNES_SetupChr();
      }
      break;

    case 0xe000:
      /* Number of 4K Banks to Number of 1K Banks  把 4K 银行数换算为 1K 银行数 */
      byte_data %= ( NesHeader.v_rom_size << 1 );
      byte_data <<= 2;

      /* Latch Control  锁存控制 */
      latch4.hi_bank = byte_data;

      if (0xfe == latch4.state)
      {
        /* Set PPU Banks  设置 PPU 银行 */
        PPUBANK[ 4 ] = VROMPAGE( byte_data );
        PPUBANK[ 5 ] = VROMPAGE( byte_data + 1 );
        PPUBANK[ 6 ] = VROMPAGE( byte_data + 2 );
        PPUBANK[ 7 ] = VROMPAGE( byte_data + 3 ); 
        InfoNES_SetupChr();
      }
      break;

    case 0xf000:
      /* Name Table Mirroring  命名表镜像 */
      InfoNES_Mirroring( byte_data & 0x01 ? 0 : 1);
      break;
  }  
}

/*-------------------------------------------------------------------*/
/*  Mapper 10 PPU Function  映射器 10：PPU 函数                      */
/*-------------------------------------------------------------------*/
void Map10_PPU( uint16_t address )
{
  /* Control Latch Selector  控制锁存选择器 */
  switch ( address & 0x3ff0 )
  {
    case 0x0fd0:
      /* Latch Control  锁存控制 */
      latch3.state = 0xfd;
      /* Set PPU Banks  设置 PPU 银行 */
      PPUBANK[ 0 ] = VROMPAGE( latch3.lo_bank );
      PPUBANK[ 1 ] = VROMPAGE( latch3.lo_bank + 1 );
      PPUBANK[ 2 ] = VROMPAGE( latch3.lo_bank + 2 );
      PPUBANK[ 3 ] = VROMPAGE( latch3.lo_bank + 3 );     
      InfoNES_SetupChr();
      break;

    case 0x0fe0:
      /* Latch Control  锁存控制 */
      latch3.state = 0xfe;
      /* Set PPU Banks  设置 PPU 银行 */
      PPUBANK[ 0 ] = VROMPAGE( latch3.hi_bank );
      PPUBANK[ 1 ] = VROMPAGE( latch3.hi_bank + 1 );
      PPUBANK[ 2 ] = VROMPAGE( latch3.hi_bank + 2 );
      PPUBANK[ 3 ] = VROMPAGE( latch3.hi_bank + 3 );     
      InfoNES_SetupChr();      
      break;

    case 0x1fd0:
      /* Latch Control  锁存控制 */
      latch4.state = 0xfd;
      /* Set PPU Banks  设置 PPU 银行 */
      PPUBANK[ 4 ] = VROMPAGE( latch4.lo_bank );
      PPUBANK[ 5 ] = VROMPAGE( latch4.lo_bank + 1 );
      PPUBANK[ 6 ] = VROMPAGE( latch4.lo_bank + 2 );
      PPUBANK[ 7 ] = VROMPAGE( latch4.lo_bank + 3 );     
      InfoNES_SetupChr();
      break;      

    case 0x1fe0:
      /* Latch Control  锁存控制 */
      latch4.state = 0xfe;
      /* Set PPU Banks  设置 PPU 银行 */
      PPUBANK[ 4 ] = VROMPAGE( latch4.hi_bank );
      PPUBANK[ 5 ] = VROMPAGE( latch4.hi_bank + 1 );
      PPUBANK[ 6 ] = VROMPAGE( latch4.hi_bank + 2 );
      PPUBANK[ 7 ] = VROMPAGE( latch4.hi_bank + 3 );     
      InfoNES_SetupChr();            
      break;
  }
}
