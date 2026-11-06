/*===================================================================*/
/*                                                                   */
/*                    Mapper 64 (Tengen RAMBO-1)  映射器 64（Tengen RAMBO-1） */
/*                                                                   */
/*===================================================================*/

uint8_t Map64_Cmd;
uint8_t Map64_Prg;
uint8_t Map64_Chr;

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 64  初始化映射器 64                            */
/*-------------------------------------------------------------------*/
void Map64_Init()
{
  /* Initialize Mapper  初始化映射器 */
  MapperInit = Map64_Init;

  /* Write to Mapper  写入映射器 */
  MapperWrite = Map64_Write;

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

  /* Set ROM Banks  设置 ROM 银行 */
  ROMBANK0 = ROMLASTPAGE( 0 );
  ROMBANK1 = ROMLASTPAGE( 0 );
  ROMBANK2 = ROMLASTPAGE( 0 );
  ROMBANK3 = ROMLASTPAGE( 0 );

  /* Set PPU Banks  设置 PPU 银行 */
  if ( NesHeader.v_rom_size > 0 )
  {
    for ( int page_number = 0; page_number < 8; ++page_number )
      PPUBANK[ page_number ] = VROMPAGE( page_number );
    InfoNES_SetupChr();
  }

  /* Initialize state flag  初始化状态标志 */
  Map64_Cmd = 0x00;
  Map64_Prg = 0x00;
  Map64_Chr = 0x00;

  /* Set up wiring of the interrupt pin  设置中断引脚接线方式 */
  K6502_Set_Int_Wiring( 1, 1 ); 
}

/*-------------------------------------------------------------------*/
/*  Mapper 64 Write Function  映射器 64：写函数                      */
/*-------------------------------------------------------------------*/
void Map64_Write( uint16_t address, uint8_t byte_data )
{
  switch ( address )
  {
    case 0x8000:
      /* Set state flag  设置状态标志 */
      Map64_Cmd = byte_data & 0x0f;
      Map64_Prg = ( byte_data & 0x40 ) >> 6;
      Map64_Chr = ( byte_data & 0x80 ) >> 7;
      break;

    case 0x8001:
      switch ( Map64_Cmd )
      {
        case 0x00:
          /* Set PPU Banks  设置 PPU 银行 */
          byte_data %= ( NesHeader.v_rom_size << 3 );
          if ( Map64_Chr )
          {
            PPUBANK[ 4 ] = VROMPAGE( byte_data );
            PPUBANK[ 5 ] = VROMPAGE( byte_data + 1 );      
          } else {
            PPUBANK[ 0 ] = VROMPAGE( byte_data );
            PPUBANK[ 1 ] = VROMPAGE( byte_data + 1 );  
          } 
          InfoNES_SetupChr();
          break;

        case 0x01:
          /* Set PPU Banks  设置 PPU 银行 */
          byte_data %= ( NesHeader.v_rom_size << 3 );
          if ( Map64_Chr )
          {
            PPUBANK[ 6 ] = VROMPAGE( byte_data );
            PPUBANK[ 7 ] = VROMPAGE( byte_data + 1 );      
          } else {
            PPUBANK[ 2 ] = VROMPAGE( byte_data );
            PPUBANK[ 3 ] = VROMPAGE( byte_data + 1 );  
          } 
          InfoNES_SetupChr();
          break;

        case 0x02:
          /* Set PPU Banks  设置 PPU 银行 */
          byte_data %= ( NesHeader.v_rom_size << 3 );
          if ( Map64_Chr )
          {
            PPUBANK[ 0 ] = VROMPAGE( byte_data );
          } else {
            PPUBANK[ 4 ] = VROMPAGE( byte_data );
          } 
          InfoNES_SetupChr();
          break;

        case 0x03:
          /* Set PPU Banks  设置 PPU 银行 */
          byte_data %= ( NesHeader.v_rom_size << 3 );
          if ( Map64_Chr )
          {
            PPUBANK[ 1 ] = VROMPAGE( byte_data );
          } else {
            PPUBANK[ 5 ] = VROMPAGE( byte_data );
          } 
          InfoNES_SetupChr();
          break;

        case 0x04:
          /* Set PPU Banks  设置 PPU 银行 */
          byte_data %= ( NesHeader.v_rom_size << 3 );
          if ( Map64_Chr )
          {
            PPUBANK[ 2 ] = VROMPAGE( byte_data );
          } else {
            PPUBANK[ 6 ] = VROMPAGE( byte_data );
          } 
          InfoNES_SetupChr();
          break;

        case 0x05:
          /* Set PPU Banks  设置 PPU 银行 */
          byte_data %= ( NesHeader.v_rom_size << 3 );
          if ( Map64_Chr )
          {
            PPUBANK[ 3 ] = VROMPAGE( byte_data );
          } else {
            PPUBANK[ 7 ] = VROMPAGE( byte_data );
          } 
          InfoNES_SetupChr();
          break;

        case 0x06:
          /* Set ROM Banks  设置 ROM 银行 */
          byte_data %= ( NesHeader.rom_size << 1 );
          if ( Map64_Prg )
          {
            ROMBANK1 = ROMPAGE( byte_data );
          } else {
            ROMBANK0 = ROMPAGE( byte_data );
          } 
          break;

        case 0x07:
          /* Set ROM Banks  设置 ROM 银行 */
          byte_data %= ( NesHeader.rom_size << 1 );
          if ( Map64_Prg )
          {
            ROMBANK2 = ROMPAGE( byte_data );
          } else {
            ROMBANK1 = ROMPAGE( byte_data );
          } 
          break;

        case 0x08:
          /* Set PPU Banks  设置 PPU 银行 */
          byte_data %= ( NesHeader.v_rom_size << 3 );
          PPUBANK[ 1 ] = VROMPAGE( byte_data );
          InfoNES_SetupChr();
          break;

        case 0x09:
          /* Set PPU Banks  设置 PPU 银行 */
          byte_data %= ( NesHeader.v_rom_size << 3 );
          PPUBANK[ 3 ] = VROMPAGE( byte_data );
          InfoNES_SetupChr();
          break;

        case 0x0f:
          /* Set ROM Banks  设置 ROM 银行 */
          byte_data %= ( NesHeader.rom_size << 1 );
          if ( Map64_Prg )
          {
            ROMBANK0 = ROMPAGE( byte_data );
          } else {
            ROMBANK2 = ROMPAGE( byte_data );
          } 
          break;
      }
      break;

    default:
      switch ( address & 0xf000 )
      {
        case 0xa000:
          /* Name Table Mirroring  命名表镜像 */
          InfoNES_Mirroring( byte_data & 0x01 );
          break;

        default:
          break;
      }
      break;
  }
}
