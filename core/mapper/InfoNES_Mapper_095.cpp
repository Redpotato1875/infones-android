/*===================================================================*/
/*                                                                   */
/*                       Mapper 95 (Namco 1??)  映射器 95（Namco 1??） */
/*                                                                   */
/*===================================================================*/

uint8_t  Map95_Regs[ 1 ];
uint32_t Map95_Prg0, Map95_Prg1;
uint32_t Map95_Chr01, Map95_Chr23;
uint32_t Map95_Chr4, Map95_Chr5, Map95_Chr6, Map95_Chr7;

#define Map95_Chr_Swap()    ( Map95_Regs[ 0 ] & 0x80 )
#define Map95_Prg_Swap()    ( Map95_Regs[ 0 ] & 0x40 )

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 95  初始化映射器 95                            */
/*-------------------------------------------------------------------*/
void Map95_Init()
{
  /* Initialize Mapper  初始化映射器 */
  MapperInit = Map95_Init;

  /* Write to Mapper  写入映射器 */
  MapperWrite = Map95_Write;

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

  /* Set SRAM Banks  设置 SRAM 银行 */
  SRAMBANK = SRAM;

  /* Initialize State Registers  初始化状态寄存器 */
  Map95_Regs[ 0 ] = 0;

  /* Set ROM Banks  设置 ROM 银行 */
  Map95_Prg0 = 0;
  Map95_Prg1 = 1;
  Map95_Set_CPU_Banks();

  /* Set PPU Banks  设置 PPU 银行 */
  if ( NesHeader.v_rom_size > 0 )
  {
    Map95_Chr01 = 0;
    Map95_Chr23 = 2;
    Map95_Chr4  = 4;
    Map95_Chr5  = 5;
    Map95_Chr6  = 6;
    Map95_Chr7  = 7;
    Map95_Set_PPU_Banks();
  } else {
    Map95_Chr01 = Map95_Chr23 = 0;
    Map95_Chr4 = Map95_Chr5 = Map95_Chr6 = Map95_Chr7 = 0;
  }

  /* Set up wiring of the interrupt pin  设置中断引脚接线方式 */
  K6502_Set_Int_Wiring( 1, 1 ); 
}

/*-------------------------------------------------------------------*/
/*  Mapper 95 Write Function  映射器 95：写函数                      */
/*-------------------------------------------------------------------*/
void Map95_Write( uint16_t address, uint8_t byte_data )
{
  uint32_t bank_num;

  switch ( address & 0xe001 )
  {
    case 0x8000:
      Map95_Regs[ 0 ] = byte_data;
      Map95_Set_PPU_Banks();
      Map95_Set_CPU_Banks();
      break;

    case 0x8001:
      if ( Map95_Regs[ 0 ] <= 0x05 )
      {
        if ( byte_data & 0x20 ) 
        {
          InfoNES_Mirroring( 2 );
        } else {
          InfoNES_Mirroring( 3 );
        }
        byte_data &= 0x1f;
      }

      bank_num = byte_data;

      switch ( Map95_Regs[ 0 ] & 0x07 )
      {
        /* Set PPU Banks  设置 PPU 银行 */
        case 0x00:
          if ( NesHeader.v_rom_size > 0 )
          {
            bank_num &= 0xfe;
            Map95_Chr01 = bank_num;
            Map95_Set_PPU_Banks();
          }
          break;

        case 0x01:
          if ( NesHeader.v_rom_size > 0 )
          {
            bank_num &= 0xfe;
            Map95_Chr23 = bank_num;
            Map95_Set_PPU_Banks();
          }
          break;

        case 0x02:
          if ( NesHeader.v_rom_size > 0 )
          {
            Map95_Chr4 = bank_num;
            Map95_Set_PPU_Banks();
          }
          break;

        case 0x03:
          if ( NesHeader.v_rom_size > 0 )
          {
            Map95_Chr5 = bank_num;
            Map95_Set_PPU_Banks();
          }
          break;

        case 0x04:
          if ( NesHeader.v_rom_size > 0 )
          {
            Map95_Chr6 = bank_num;
            Map95_Set_PPU_Banks();
          }
          break;

        case 0x05:
          if ( NesHeader.v_rom_size > 0 )
          {
            Map95_Chr7 = bank_num;
            Map95_Set_PPU_Banks();
          }
          break;

        /* Set ROM Banks  设置 ROM 银行 */
        case 0x06:
          Map95_Prg0 = bank_num;
          Map95_Set_CPU_Banks();
          break;

        case 0x07:
          Map95_Prg1 = bank_num;
          Map95_Set_CPU_Banks();
          break;
      }
      break;
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 95 Set CPU Banks Function  映射器 95：设置 CPU 银行函数   */
/*-------------------------------------------------------------------*/
void Map95_Set_CPU_Banks()
{
  if ( Map95_Prg_Swap() )
  {
    ROMBANK0 = ROMLASTPAGE( 1 );
    ROMBANK1 = ROMPAGE( Map95_Prg1 % ( NesHeader.rom_size << 1 ) );
    ROMBANK2 = ROMPAGE( Map95_Prg0 % ( NesHeader.rom_size << 1 ) );
    ROMBANK3 = ROMLASTPAGE( 0 );
  } else {
    ROMBANK0 = ROMPAGE( Map95_Prg0 % ( NesHeader.rom_size << 1 ) );
    ROMBANK1 = ROMPAGE( Map95_Prg1 % ( NesHeader.rom_size << 1 ) );
    ROMBANK2 = ROMLASTPAGE( 1 );
    ROMBANK3 = ROMLASTPAGE( 0 );
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 95 Set PPU Banks Function  映射器 95：设置 PPU 银行函数   */
/*-------------------------------------------------------------------*/
void Map95_Set_PPU_Banks()
{
  if ( NesHeader.v_rom_size > 0 )
  {
    if ( Map95_Chr_Swap() )
    { 
      PPUBANK[ 0 ] = VROMPAGE( Map95_Chr4 % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 1 ] = VROMPAGE( Map95_Chr5 % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 2 ] = VROMPAGE( Map95_Chr6 % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 3 ] = VROMPAGE( Map95_Chr7 % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 4 ] = VROMPAGE( ( Map95_Chr01 + 0 ) % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 5 ] = VROMPAGE( ( Map95_Chr01 + 1 ) % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 6 ] = VROMPAGE( ( Map95_Chr23 + 0 ) % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 7 ] = VROMPAGE( ( Map95_Chr23 + 1 ) % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();
    } else {
      PPUBANK[ 0 ] = VROMPAGE( ( Map95_Chr01 + 0 ) % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 1 ] = VROMPAGE( ( Map95_Chr01 + 1 ) % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 2 ] = VROMPAGE( ( Map95_Chr23 + 0 ) % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 3 ] = VROMPAGE( ( Map95_Chr23 + 1 ) % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 4 ] = VROMPAGE( Map95_Chr4 % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 5 ] = VROMPAGE( Map95_Chr5 % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 6 ] = VROMPAGE( Map95_Chr6 % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 7 ] = VROMPAGE( Map95_Chr7 % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();
    }
  }
}
