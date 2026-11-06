/*===================================================================*/
/*                                                                   */
/*                        Mapper 90 (PC-JY-??)  映射器 90（PC-JY-??） */
/*                                                                   */
/*===================================================================*/

uint8_t Map90_Prg_Reg[ 4 ];
uint8_t Map90_Chr_Low_Reg[ 8 ];
uint8_t Map90_Chr_High_Reg[ 8 ];
uint8_t Map90_Nam_Low_Reg[ 4 ];
uint8_t Map90_Nam_High_Reg[ 4 ];

uint8_t Map90_Prg_Bank_Size;
uint8_t Map90_Prg_Bank_6000;
uint8_t Map90_Prg_Bank_E000;
uint8_t Map90_Chr_Bank_Size;
uint8_t Map90_Mirror_Mode;
uint8_t Map90_Mirror_Type;

uint32_t Map90_Value1;
uint32_t Map90_Value2;

uint8_t Map90_IRQ_Enable;
uint8_t Map90_IRQ_Cnt;
uint8_t Map90_IRQ_Latch;

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 90  初始化映射器 90                            */
/*-------------------------------------------------------------------*/
void Map90_Init()
{
  /* Initialize Mapper  初始化映射器 */
  MapperInit = Map90_Init;

  /* Write to Mapper  写入映射器 */
  MapperWrite = Map90_Write;

  /* Write to SRAM  写入 SRAM */
  MapperSram = Map0_Sram;

  /* Write to APU  写入 APU */
  MapperApu = Map90_Apu;

  /* Read from APU  从 APU 读取 */
  MapperReadApu = Map90_ReadApu;

  /* Callback at VSync  场同步回调 */
  MapperVSync = Map0_VSync;

  /* Callback at HSync  行同步回调 */
  MapperHSync = Map90_HSync;

  /* Callback at PPU  PPU 回调 */
  MapperPPU = Map0_PPU;

  /* Callback at Rendering Screen ( 1:BG, 0:Sprite )  渲染画面回调（1：背景，0：精灵） */
  MapperRenderScreen = Map0_RenderScreen;

  /* Set SRAM Banks  设置 SRAM 银行 */
  SRAMBANK = SRAM;

  /* Set ROM Banks  设置 ROM 银行 */
  ROMBANK0 = ROMLASTPAGE( 3 );
  ROMBANK1 = ROMLASTPAGE( 2 );
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
  Map90_IRQ_Cnt = 0;
  Map90_IRQ_Latch = 0;
  Map90_IRQ_Enable = 0;

  for ( uint8_t page_number = 0; page_number < 4; page_number++ )
  {
    Map90_Prg_Reg[ page_number ] = ( NesHeader.rom_size << 1 ) - 4 + page_number;
    Map90_Nam_Low_Reg[ page_number ] = 0;
    Map90_Nam_High_Reg[ page_number ] = 0;
    Map90_Chr_Low_Reg[ page_number ] = page_number;
    Map90_Chr_High_Reg[ page_number ] = 0;
    Map90_Chr_Low_Reg[ page_number + 4 ] = page_number + 4;
    Map90_Chr_High_Reg[ page_number + 4 ] = 0;
  }

  /* Set up wiring of the interrupt pin  设置中断引脚接线方式 */
  K6502_Set_Int_Wiring( 1, 1 ); 
}

/*-------------------------------------------------------------------*/
/*  Mapper 90 Write to APU Function  映射器 90：写 APU 函数          */
/*-------------------------------------------------------------------*/
void Map90_Apu( uint16_t address, uint8_t byte_data )
{
  switch ( address )
  {
    case 0x5000:
      Map90_Value1 = byte_data;
      break;

    case 0x5001:
      Map90_Value2 = byte_data;
      break;
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 90 Read from APU Function  映射器 90：读 APU 函数         */
/*-------------------------------------------------------------------*/
uint8_t Map90_ReadApu( uint16_t address )
{
  if ( address == 0x5000 )
  {
    return ( uint8_t )( ( Map90_Value1 * Map90_Value2 ) & 0x00ff );
  } else {
    return ( uint8_t )( address >> 8 );
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 90 Write Function  映射器 90：写函数                      */
/*-------------------------------------------------------------------*/
void Map90_Write( uint16_t address, uint8_t byte_data )
{
  switch ( address )
  {
    /* Set ROM Banks  设置 ROM 银行 */
    case 0x8000:
    case 0x8001:
    case 0x8002:
    case 0x8003:
      Map90_Prg_Reg[ address & 0x03 ] = byte_data;
      Map90_Sync_Prg_Banks();
      break;
    
    case 0x9000:
    case 0x9001:
    case 0x9002:
    case 0x9003:
    case 0x9004:
    case 0x9005:
    case 0x9006:
    case 0x9007:
      Map90_Chr_Low_Reg[ address & 0x07 ] = byte_data;
      Map90_Sync_Chr_Banks();
      break;

    case 0xa000:
    case 0xa001:
    case 0xa002:
    case 0xa003:
    case 0xa004:
    case 0xa005:
    case 0xa006:
    case 0xa007:
      Map90_Chr_High_Reg[ address & 0x07 ] = byte_data;
      Map90_Sync_Chr_Banks();
      break;

    case 0xb000:
    case 0xb001:
    case 0xb002:
    case 0xb003:
      Map90_Nam_Low_Reg[ address & 0x03 ] = byte_data;
      Map90_Sync_Mirror();
      break;

    case 0xb004:
    case 0xb005:
    case 0xb006:
    case 0xb007:
      Map90_Nam_High_Reg[ address & 0x03 ] = byte_data;
      Map90_Sync_Mirror();
      break;

    case 0xc002:
      Map90_IRQ_Enable = 0;
      break;

    case 0xc003:
    case 0xc004:
      if ( Map90_IRQ_Enable == 0 )
      {
        Map90_IRQ_Enable = 1;
        Map90_IRQ_Cnt = Map90_IRQ_Latch;
      }
      break;

    case 0xc005:
      Map90_IRQ_Cnt = byte_data;
      Map90_IRQ_Latch = byte_data;
      break;

    case 0xd000:
      Map90_Prg_Bank_6000 = byte_data & 0x80;
      Map90_Prg_Bank_E000 = byte_data & 0x04;
      Map90_Prg_Bank_Size = byte_data & 0x03;
      Map90_Chr_Bank_Size = ( byte_data & 0x18 ) >> 3;
      Map90_Mirror_Mode = byte_data & 0x20;
      
      Map90_Sync_Prg_Banks();
      Map90_Sync_Chr_Banks();
      Map90_Sync_Mirror();
      break;

    case 0xd001:
      Map90_Mirror_Type = byte_data & 0x03;
      Map90_Sync_Mirror();
      break;

    case 0xd003:
      /* Bank Page*/
      break;
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 90 H-Sync Function  映射器 90：行同步函数                 */
/*-------------------------------------------------------------------*/
void Map90_HSync()
{
/*
 *  Callback at HSync  行同步回调
 *
 */
  if ( 0 <= PPU_Scanline && PPU_Scanline <= 239 )
  {
    if ( PPU_R1 & R1_SHOW_SCR || PPU_R1 & R1_SHOW_SP )
    {
      if ( Map90_IRQ_Cnt == 0 )
      {
        if ( Map90_IRQ_Enable )
        {
          IRQ_REQ;
        }
        Map90_IRQ_Latch = 0;
        Map90_IRQ_Enable = 0;
      } else {
        Map90_IRQ_Cnt--;
      }
    }
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 90 Sync Mirror Function  映射器 90：同步镜像函数          */
/*-------------------------------------------------------------------*/
void Map90_Sync_Mirror( void )
{
  uint8_t page_number;
  uint32_t nam_bank[ 4 ];

  for ( page_number = 0; page_number < 4; page_number++ )
  {
    nam_bank[ page_number ] = 
      ( (uint32_t)Map90_Nam_High_Reg[ page_number ] << 8 ) | (uint32_t)Map90_Nam_Low_Reg[ page_number ];
  }

  if ( Map90_Mirror_Mode )
  {
    for ( page_number = 0; page_number < 4; page_number++ )
    {
      if ( !Map90_Nam_High_Reg[ page_number ] && ( Map90_Nam_Low_Reg[ page_number ] == page_number ) )
      {
        Map90_Mirror_Mode = 0;
      }
    }

    if ( Map90_Mirror_Mode )
    {
        PPUBANK[ NAME_TABLE0 ] = VROMPAGE( nam_bank[ 0 ] % ( NesHeader.v_rom_size << 3 ) );
        PPUBANK[ NAME_TABLE1 ] = VROMPAGE( nam_bank[ 1 ] % ( NesHeader.v_rom_size << 3 ) );
        PPUBANK[ NAME_TABLE2 ] = VROMPAGE( nam_bank[ 2 ] % ( NesHeader.v_rom_size << 3 ) );
        PPUBANK[ NAME_TABLE3 ] = VROMPAGE( nam_bank[ 3 ] % ( NesHeader.v_rom_size << 3 ) );
    }
  } else {
    switch ( Map90_Mirror_Type )
    {
      case 0x00:
        InfoNES_Mirroring( 1 );
        break;

      case 0x01:
        InfoNES_Mirroring( 0 );
        break;

      default:
        InfoNES_Mirroring( 3 );
        break;
    }
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 90 Sync Char Banks Function  映射器 90：同步字符银行函数  */
/*-------------------------------------------------------------------*/
void Map90_Sync_Chr_Banks( void )
{
  uint8_t page_number;
  uint32_t chr_bank[ 8 ];

  for ( page_number = 0; page_number < 8; page_number++ )
  {
    chr_bank[ page_number ] =
      ( (uint32_t)Map90_Chr_High_Reg[ page_number ] << 8 ) | (uint32_t)Map90_Chr_Low_Reg[ page_number ];
  }

  switch ( Map90_Chr_Bank_Size )
  {
    case 0:
      PPUBANK[ 0 ] = VROMPAGE( ( ( chr_bank[ 0 ] << 3 ) + 0 ) % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 1 ] = VROMPAGE( ( ( chr_bank[ 0 ] << 3 ) + 1 ) % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 2 ] = VROMPAGE( ( ( chr_bank[ 0 ] << 3 ) + 2 ) % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 3 ] = VROMPAGE( ( ( chr_bank[ 0 ] << 3 ) + 3 ) % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 4 ] = VROMPAGE( ( ( chr_bank[ 0 ] << 3 ) + 4 ) % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 5 ] = VROMPAGE( ( ( chr_bank[ 0 ] << 3 ) + 5 ) % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 6 ] = VROMPAGE( ( ( chr_bank[ 0 ] << 3 ) + 6 ) % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 7 ] = VROMPAGE( ( ( chr_bank[ 0 ] << 3 ) + 7 ) % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();
      break;

    case 1:
      PPUBANK[ 0 ] = VROMPAGE( ( ( chr_bank[ 0 ] << 2 ) + 0 ) % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 1 ] = VROMPAGE( ( ( chr_bank[ 0 ] << 2 ) + 1 ) % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 2 ] = VROMPAGE( ( ( chr_bank[ 0 ] << 2 ) + 2 ) % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 3 ] = VROMPAGE( ( ( chr_bank[ 0 ] << 2 ) + 3 ) % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 4 ] = VROMPAGE( ( ( chr_bank[ 4 ] << 2 ) + 0 ) % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 5 ] = VROMPAGE( ( ( chr_bank[ 4 ] << 2 ) + 1 ) % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 6 ] = VROMPAGE( ( ( chr_bank[ 4 ] << 2 ) + 2 ) % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 7 ] = VROMPAGE( ( ( chr_bank[ 4 ] << 2 ) + 3 ) % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();
      break;

    case 2:
      PPUBANK[ 0 ] = VROMPAGE( ( ( chr_bank[ 0 ] << 1 ) + 0 ) % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 1 ] = VROMPAGE( ( ( chr_bank[ 0 ] << 1 ) + 1 ) % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 2 ] = VROMPAGE( ( ( chr_bank[ 2 ] << 1 ) + 0 ) % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 3 ] = VROMPAGE( ( ( chr_bank[ 2 ] << 1 ) + 1 ) % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 4 ] = VROMPAGE( ( ( chr_bank[ 4 ] << 1 ) + 0 ) % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 5 ] = VROMPAGE( ( ( chr_bank[ 4 ] << 1 ) + 1 ) % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 6 ] = VROMPAGE( ( ( chr_bank[ 6 ] << 1 ) + 0 ) % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 7 ] = VROMPAGE( ( ( chr_bank[ 6 ] << 1 ) + 1 ) % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();
      break;

    default:
      PPUBANK[ 0 ] = VROMPAGE( chr_bank[ 0 ] % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 1 ] = VROMPAGE( chr_bank[ 1 ] % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 2 ] = VROMPAGE( chr_bank[ 2 ] % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 3 ] = VROMPAGE( chr_bank[ 3 ] % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 4 ] = VROMPAGE( chr_bank[ 4 ] % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 5 ] = VROMPAGE( chr_bank[ 5 ] % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 6 ] = VROMPAGE( chr_bank[ 6 ] % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 7 ] = VROMPAGE( chr_bank[ 7 ] % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();
      break;
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 90 Sync Program Banks Function  映射器 90：同步程序银行函数 */
/*-------------------------------------------------------------------*/
void Map90_Sync_Prg_Banks( void )
{
  switch ( Map90_Prg_Bank_Size )
  {
    case 0:
      ROMBANK0 = ROMLASTPAGE( 3 );
      ROMBANK1 = ROMLASTPAGE( 2 );
      ROMBANK2 = ROMLASTPAGE( 1 );
      ROMBANK3 = ROMLASTPAGE( 0 );      
      break;

    case 1:
      ROMBANK0 = ROMPAGE( ( ( Map90_Prg_Reg[ 1 ] << 1 ) + 0 ) % ( NesHeader.rom_size << 1 ) );
      ROMBANK1 = ROMPAGE( ( ( Map90_Prg_Reg[ 1 ] << 1 ) + 1 ) % ( NesHeader.rom_size << 1 ) );
      ROMBANK2 = ROMLASTPAGE( 1 );
      ROMBANK3 = ROMLASTPAGE( 0 );      
      break;

    case 2:
      if ( Map90_Prg_Bank_E000 )
      {
        ROMBANK0 = ROMPAGE( Map90_Prg_Reg[ 0 ] % ( NesHeader.rom_size << 1 ) );
        ROMBANK1 = ROMPAGE( Map90_Prg_Reg[ 1 ] % ( NesHeader.rom_size << 1 ) );
        ROMBANK2 = ROMPAGE( Map90_Prg_Reg[ 2 ] % ( NesHeader.rom_size << 1 ) );
        ROMBANK3 = ROMPAGE( Map90_Prg_Reg[ 3 ] % ( NesHeader.rom_size << 1 ) );   
      } else {
        if ( Map90_Prg_Bank_6000 )
        {
          SRAMBANK = ROMPAGE( Map90_Prg_Reg[ 3 ] % ( NesHeader.rom_size << 1 ) );
        }
        ROMBANK0 = ROMPAGE( Map90_Prg_Reg[ 0 ] % ( NesHeader.rom_size << 1 ) );
        ROMBANK1 = ROMPAGE( Map90_Prg_Reg[ 1 ] % ( NesHeader.rom_size << 1 ) );
        ROMBANK2 = ROMPAGE( Map90_Prg_Reg[ 2 ] % ( NesHeader.rom_size << 1 ) );
        ROMBANK3 = ROMLASTPAGE( 0 );  
      }
      break;

    default:
      /* 8k in reverse mode?  是否为反向模式的 8k？ */
      ROMBANK0 = ROMPAGE( Map90_Prg_Reg[ 3 ] % ( NesHeader.rom_size << 1 ) );
      ROMBANK1 = ROMPAGE( Map90_Prg_Reg[ 2 ] % ( NesHeader.rom_size << 1 ) );
      ROMBANK2 = ROMPAGE( Map90_Prg_Reg[ 1 ] % ( NesHeader.rom_size << 1 ) );
      ROMBANK3 = ROMPAGE( Map90_Prg_Reg[ 0 ] % ( NesHeader.rom_size << 1 ) ); 
      break;
  }
}
