/*===================================================================*/
/*                                                                   */
/*                     Mapper 9 (MMC2)  映射器 9（MMC2）             */
/*                                                                   */
/*===================================================================*/

struct Map9_Latch 
{
  uint8_t lo_bank;
  uint8_t hi_bank;
  uint8_t state;
};

struct Map9_Latch latch1;
struct Map9_Latch latch2;

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 9  初始化映射器 9                              */
/*-------------------------------------------------------------------*/
void Map9_Init()
{
  int page_number;

  /* Initialize Mapper  初始化映射器 */
  MapperInit = Map9_Init;

  /* Write to Mapper  写入映射器 */
  MapperWrite = Map9_Write;

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
  MapperPPU = Map9_PPU;

  /* Callback at Rendering Screen ( 1:BG, 0:Sprite )  渲染画面回调（1：背景，0：精灵） */
  MapperRenderScreen = Map0_RenderScreen;

  /* Set SRAM Banks  设置 SRAM 银行 */
  SRAMBANK = SRAM;

  /* Set ROM Banks  设置 ROM 银行 */
  ROMBANK0 = ROMPAGE( 0 );
  ROMBANK1 = ROMLASTPAGE( 2 );
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
  latch1.state = 0xfe;
  latch1.lo_bank = 0;
  latch1.hi_bank = 0;
  latch2.state = 0xfe;
  latch2.lo_bank = 0;
  latch2.hi_bank = 0;

  /* Set up wiring of the interrupt pin  设置中断引脚接线方式 */
  K6502_Set_Int_Wiring( 1, 1 );
}

/*-------------------------------------------------------------------*/
/*  Mapper 9 Write Function  映射器 9：写函数                        */
/*-------------------------------------------------------------------*/
void Map9_Write( uint16_t address, uint8_t byte_data )
{
  uint16_t map_addr;

  map_addr = address & 0xf000;
  switch ( map_addr )
  {
    case 0xa000:
      /* Set ROM Banks  设置 ROM 银行 */
      byte_data %= ( NesHeader.rom_size << 1 );
      ROMBANK0 = ROMPAGE( byte_data );
      break;

    case 0xb000:
      /* Number of 4K Banks to Number of 1K Banks  把 4K 银行数换算为 1K 银行数 */
      byte_data %= ( NesHeader.v_rom_size << 1 );
      byte_data <<= 2;

      /* Latch Control  锁存控制 */
      latch1.lo_bank = byte_data;

      if (0xfd == latch1.state)
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
      latch1.hi_bank = byte_data;

      if (0xfe == latch1.state)
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
      latch2.lo_bank = byte_data;

      if (0xfd == latch2.state)
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
      latch2.hi_bank = byte_data;

      if (0xfe == latch2.state)
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
/*  Mapper 9 PPU Function  映射器 9：PPU 函数                        */
/*-------------------------------------------------------------------*/
void Map9_PPU( uint16_t address )
{
  /* Control Latch Selector  控制锁存选择器 */
  switch ( address & 0x3ff0 )
  {
    case 0x0fd0:
      /* Latch Control  锁存控制 */
      latch1.state = 0xfd;
      /* Set PPU Banks  设置 PPU 银行 */
      PPUBANK[ 0 ] = VROMPAGE( latch1.lo_bank );
      PPUBANK[ 1 ] = VROMPAGE( latch1.lo_bank + 1 );
      PPUBANK[ 2 ] = VROMPAGE( latch1.lo_bank + 2 );
      PPUBANK[ 3 ] = VROMPAGE( latch1.lo_bank + 3 );     
      InfoNES_SetupChr();
      break;

    case 0x0fe0:
      /* Latch Control  锁存控制 */
      latch1.state = 0xfe;
      /* Set PPU Banks  设置 PPU 银行 */
      PPUBANK[ 0 ] = VROMPAGE( latch1.hi_bank );
      PPUBANK[ 1 ] = VROMPAGE( latch1.hi_bank + 1 );
      PPUBANK[ 2 ] = VROMPAGE( latch1.hi_bank + 2 );
      PPUBANK[ 3 ] = VROMPAGE( latch1.hi_bank + 3 );     
      InfoNES_SetupChr();      
      break;

    case 0x1fd0:
      /* Latch Control  锁存控制 */
      latch2.state = 0xfd;
      /* Set PPU Banks  设置 PPU 银行 */
      PPUBANK[ 4 ] = VROMPAGE( latch2.lo_bank );
      PPUBANK[ 5 ] = VROMPAGE( latch2.lo_bank + 1 );
      PPUBANK[ 6 ] = VROMPAGE( latch2.lo_bank + 2 );
      PPUBANK[ 7 ] = VROMPAGE( latch2.lo_bank + 3 );     
      InfoNES_SetupChr();
      break;      

    case 0x1fe0:
      /* Latch Control  锁存控制 */
      latch2.state = 0xfe;
      /* Set PPU Banks  设置 PPU 银行 */
      PPUBANK[ 4 ] = VROMPAGE( latch2.hi_bank );
      PPUBANK[ 5 ] = VROMPAGE( latch2.hi_bank + 1 );
      PPUBANK[ 6 ] = VROMPAGE( latch2.hi_bank + 2 );
      PPUBANK[ 7 ] = VROMPAGE( latch2.hi_bank + 3 );     
      InfoNES_SetupChr();            
      break;
  }
}
