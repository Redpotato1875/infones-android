/*===================================================================*/
/*                                                                   */
/*                        Mapper 160 (Pirates)  映射器 160（Pirates） */
/*                                                                   */
/*===================================================================*/

uint8_t Map160_IRQ_Enable;
uint8_t Map160_IRQ_Cnt;
uint8_t Map160_IRQ_Latch;
uint8_t Map160_Refresh_Type;

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 160  初始化映射器 160                          */
/*-------------------------------------------------------------------*/
void Map160_Init()
{
  /* Initialize Mapper  初始化映射器 */
  MapperInit = Map160_Init;

  /* Write to Mapper  写入映射器 */
  MapperWrite = Map160_Write;

  /* Write to SRAM  写入 SRAM */
  MapperSram = Map0_Sram;

  /* Write to APU  写入 APU */
  MapperApu = Map0_Apu;

  /* Read from APU  从 APU 读取 */
  MapperReadApu = Map0_ReadApu;

  /* Callback at VSync  场同步回调 */
  MapperVSync = Map0_VSync;

  /* Callback at HSync  行同步回调 */
  MapperHSync = Map160_HSync;

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
  Map160_IRQ_Enable = 0;
  Map160_IRQ_Cnt = 0;
  Map160_IRQ_Latch = 0;
  Map160_Refresh_Type = 0;

  /* Set up wiring of the interrupt pin  设置中断引脚接线方式 */
  K6502_Set_Int_Wiring( 1, 1 ); 
}

/*-------------------------------------------------------------------*/
/*  Mapper 160 Write Function  映射器 160：写函数                    */
/*-------------------------------------------------------------------*/
void Map160_Write( uint16_t address, uint8_t byte_data )
{
  switch ( address )
  {
    /* Set ROM Banks  设置 ROM 银行 */
    case 0x8000:
      ROMBANK0 = ROMPAGE( byte_data % ( NesHeader.rom_size << 1 ) );
      break;

    case 0x8001:
      ROMBANK1 = ROMPAGE( byte_data % ( NesHeader.rom_size << 1 ) );
      break;

    case 0x8002:
      ROMBANK2 = ROMPAGE( byte_data % ( NesHeader.rom_size << 1 ) );
      break;

    case 0x9000:
      if ( byte_data == 0x2b )
      {
        Map160_Refresh_Type = 1;      /* Title  标题画面 */
      }
      else if ( byte_data == 0xa8 )
      {
        Map160_Refresh_Type = 2;      /* Passwd  密码画面 */
      }
      else if ( byte_data == 0x1f )
      {
        Map160_Refresh_Type = 3;      /* Game Over  游戏结束画面 */
      }
      else if ( byte_data == 0x7c )
      {
        Map160_Refresh_Type = 4;      /* Continue  继续 */
      }
      else if ( byte_data == 0x18 )
      {
        Map160_Refresh_Type = 5;      /* Roulette  轮盘画面 */
      }
      else if ( byte_data == 0x60 )
      {
        Map160_Refresh_Type = 6;      /* Congratulation  通关祝贺画面 */
      }     
      else
      {
        Map160_Refresh_Type = 0;   
      }

      PPUBANK[ 0 ] = VROMPAGE( byte_data % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();
      break;

    case 0x9001:
      PPUBANK[ 1 ] = VROMPAGE( byte_data % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();
      break;

    case 0x9002:
      if ( Map160_Refresh_Type == 2 && byte_data != 0xe8 )
      {
        Map160_Refresh_Type = 0;     /* Not Passwd  非密码画面 */
      }
      PPUBANK[ 2 ] = VROMPAGE( byte_data % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();
      break;

    case 0x9003:
      PPUBANK[ 3 ] = VROMPAGE( byte_data % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();
      break;

    case 0x9004:
      PPUBANK[ 4 ] = VROMPAGE( byte_data % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();
      break;
  
    case 0x9005:
      PPUBANK[ 5 ] = VROMPAGE( byte_data % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();
      break;

    case 0x9006:
      PPUBANK[ 6 ] = VROMPAGE( byte_data % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();
      break;

    case 0x9007:
      PPUBANK[ 7 ] = VROMPAGE( byte_data % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();
      break;      

    case 0xc000:
      Map160_IRQ_Cnt = Map160_IRQ_Latch;
      Map160_IRQ_Enable = Map160_IRQ_Latch;
      break;

    case 0xc001:
      Map160_IRQ_Latch = byte_data;
      break;

    case 0xc002:
      Map160_IRQ_Enable = 0;
      break;

    case 0xc003:
      Map160_IRQ_Cnt = byte_data;
      break;
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 160 H-Sync Function  映射器 160：行同步函数               */
/*-------------------------------------------------------------------*/
void Map160_HSync()
{
/*
 *  Callback at HSync  行同步回调
 *
 */
  if ( PPU_Scanline == 0 || PPU_Scanline == 239 )
  {
    if ( Map160_Refresh_Type == 1 )
    {
      PPUBANK[ 0 ] = VROMPAGE( 0x58 % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 1 ] = VROMPAGE( 0x59 % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 2 ] = VROMPAGE( 0x5a % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 3 ] = VROMPAGE( 0x5b % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 4 ] = VROMPAGE( 0x58 % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 5 ] = VROMPAGE( 0x59 % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 6 ] = VROMPAGE( 0x5a % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 7 ] = VROMPAGE( 0x5b % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();
    }
    else if ( Map160_Refresh_Type == 2 )
    {
      PPUBANK[ 0 ] = VROMPAGE( 0x78 % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 1 ] = VROMPAGE( 0x79 % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 2 ] = VROMPAGE( 0x7a % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 3 ] = VROMPAGE( 0x7b % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 4 ] = VROMPAGE( 0x78 % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 5 ] = VROMPAGE( 0x79 % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 6 ] = VROMPAGE( 0x7a % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 7 ] = VROMPAGE( 0x7b % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();
    }
    else if ( Map160_Refresh_Type == 3 )
    {
      PPUBANK[ 0 ] = VROMPAGE( 0x7c % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 1 ] = VROMPAGE( 0x7d % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 2 ] = VROMPAGE( 0x7e % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 3 ] = VROMPAGE( 0x7f % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 4 ] = VROMPAGE( 0x7c % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 5 ] = VROMPAGE( 0x7d % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 6 ] = VROMPAGE( 0x7e % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 7 ] = VROMPAGE( 0x7f % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();
    }
    else if ( Map160_Refresh_Type == 5 )
    {
      PPUBANK[ 0 ] = VROMPAGE( 0x70 % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 1 ] = VROMPAGE( 0x71 % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 2 ] = VROMPAGE( 0x72 % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 3 ] = VROMPAGE( 0x73 % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 4 ] = VROMPAGE( 0x74 % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 5 ] = VROMPAGE( 0x75 % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 6 ] = VROMPAGE( 0x76 % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 7 ] = VROMPAGE( 0x77 % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();
    }
    else if ( Map160_Refresh_Type == 6 )
    {
      PPUBANK[ 0 ] = VROMPAGE( 0x5c % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 1 ] = VROMPAGE( 0x5d % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 2 ] = VROMPAGE( 0x5e % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 3 ] = VROMPAGE( 0x5f % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 4 ] = VROMPAGE( 0x7c % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 5 ] = VROMPAGE( 0x7d % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 6 ] = VROMPAGE( 0x7e % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 7 ] = VROMPAGE( 0x7f % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();
    }
  }

  if ( PPU_Scanline == 64 )
  {
    if ( Map160_Refresh_Type == 4 )
    {
      PPUBANK[ 0 ] = VROMPAGE( 0x6c % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 1 ] = VROMPAGE( 0x6d % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 2 ] = VROMPAGE( 0x6e % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 3 ] = VROMPAGE( 0x6f % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();
    }
  }

  if ( PPU_Scanline == 128 )
  {
    if ( Map160_Refresh_Type == 4 )
    {
      PPUBANK[ 0 ] = VROMPAGE( 0x68 % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 1 ] = VROMPAGE( 0x69 % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 2 ] = VROMPAGE( 0x6a % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 3 ] = VROMPAGE( 0x6b % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();
    }
    else if ( Map160_Refresh_Type == 5 )
    {
      PPUBANK[ 0 ] = VROMPAGE( 0x74 % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 1 ] = VROMPAGE( 0x75 % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 2 ] = VROMPAGE( 0x76 % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 3 ] = VROMPAGE( 0x77 % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 4 ] = VROMPAGE( 0x74 % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 5 ] = VROMPAGE( 0x75 % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 6 ] = VROMPAGE( 0x76 % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 7 ] = VROMPAGE( 0x77 % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();
    }
  }

  if ( PPU_Scanline == 160 )
  {
    if ( Map160_Refresh_Type == 6 )
    {
      PPUBANK[ 0 ] = VROMPAGE( 0x60 % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 1 ] = VROMPAGE( 0x61 % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 2 ] = VROMPAGE( 0x5e % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 3 ] = VROMPAGE( 0x5f % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 4 ] = VROMPAGE( 0x7c % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 5 ] = VROMPAGE( 0x7d % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 6 ] = VROMPAGE( 0x7e % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 7 ] = VROMPAGE( 0x7f % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();
    }
  }

  if ( Map160_IRQ_Enable )
  {
    if ( Map160_IRQ_Cnt == 0xff )
    {
      IRQ_REQ;
      Map160_IRQ_Cnt = 0;
      Map160_IRQ_Enable = 0;
    } else {
      Map160_IRQ_Cnt++;
    }
  }
}
