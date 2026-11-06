/*===================================================================*/
/*                                                                   */
/*                        Mapper 188 (Bandai)  映射器 188（Bandai）  */
/*                                                                   */
/*===================================================================*/

uint8_t Map188_Dummy[ 0x2000 ];

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 188  初始化映射器 188                          */
/*-------------------------------------------------------------------*/
void Map188_Init()
{
  /* Initialize Mapper  初始化映射器 */
  MapperInit = Map188_Init;

  /* Write to Mapper  写入映射器 */
  MapperWrite = Map188_Write;

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
  SRAMBANK = Map188_Dummy;

  /* Set ROM Banks  设置 ROM 银行 */
  if ( ( NesHeader.rom_size << 1 ) > 16 )
  {
    ROMBANK0 = ROMPAGE( 0 );
    ROMBANK1 = ROMPAGE( 1 );
    ROMBANK2 = ROMPAGE( 14 );
    ROMBANK3 = ROMPAGE( 15 );
  } else {
    ROMBANK0 = ROMPAGE( 0 );
    ROMBANK1 = ROMPAGE( 1 );
    ROMBANK2 = ROMLASTPAGE( 1 );
    ROMBANK3 = ROMLASTPAGE( 0 );
  }

  /* Magic Code  魔法密码 */
  Map188_Dummy[ 0 ] = 0x03;

  /* Set up wiring of the interrupt pin  设置中断引脚接线方式 */
  K6502_Set_Int_Wiring( 1, 1 ); 
}

/*-------------------------------------------------------------------*/
/*  Mapper 188 Write Function  映射器 188：写函数                    */
/*-------------------------------------------------------------------*/
void Map188_Write( uint16_t address, uint8_t byte_data )
{
  /* Set ROM Banks  设置 ROM 银行 */
  if ( byte_data )
  {
    if ( byte_data & 0x10 )
    {
      byte_data = ( byte_data & 0x07 ) << 1;
      ROMBANK0 = ROMPAGE( ( byte_data + 0 ) % ( NesHeader.rom_size << 1 ) );
      ROMBANK1 = ROMPAGE( ( byte_data + 1 ) % ( NesHeader.rom_size << 1 ) );
    } else {
      byte_data <<= 1;
      ROMBANK0 = ROMPAGE( ( byte_data + 16 ) % ( NesHeader.rom_size << 1 ) );
      ROMBANK1 = ROMPAGE( ( byte_data + 17 ) % ( NesHeader.rom_size << 1 ) );
    }
  } 
  else 
  {
    if ( ( NesHeader.rom_size << 1 ) == 0x10 )
    {
      ROMBANK0 = ROMPAGE( 14 );
      ROMBANK1 = ROMPAGE( 15 );
    } else {
      ROMBANK0 = ROMPAGE( 16 );
      ROMBANK1 = ROMPAGE( 17 );
    }
  }
}
