/*===================================================================*/
/*                                                                   */
/*                      Mapper 243 (Pirates)  映射器 243（Pirates）  */
/*                                                                   */
/*===================================================================*/

uint8_t Map243_Regs[4];

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 243  初始化映射器 243                          */
/*-------------------------------------------------------------------*/
void Map243_Init()
{
  /* Initialize Mapper  初始化映射器 */
  MapperInit = Map243_Init;

  /* Write to Mapper  写入映射器 */
  MapperWrite = Map0_Write;

  /* Write to SRAM  写入 SRAM */
  MapperSram = Map0_Sram;

  /* Write to APU  写入 APU */
  MapperApu = Map243_Apu;

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
  ROMBANK0 = ROMPAGE( 0 );
  ROMBANK1 = ROMPAGE( 1 );
  ROMBANK2 = ROMPAGE( 2 );
  ROMBANK3 = ROMPAGE( 3 );

  /* Set PPU Banks  设置 PPU 银行 */
  if ( NesHeader.v_rom_size > 0 )
  {
    for ( int page_number = 0; page_number < 8; ++page_number )
    {
      PPUBANK[ page_number ] = VROMPAGE( page_number );
    }
    InfoNES_SetupChr();
  }

  /* Initialize state registers  初始化状态寄存器 */
  Map243_Regs[0] = Map243_Regs[1] = Map243_Regs[2] = Map243_Regs[3] = 0;

  /* Set up wiring of the interrupt pin  设置中断引脚接线方式 */
  K6502_Set_Int_Wiring( 1, 1 ); 
}

/*-------------------------------------------------------------------*/
/*  Mapper 243 Write to Apu Function  映射器 243：写 APU 函数        */
/*-------------------------------------------------------------------*/
void Map243_Apu( uint16_t address, uint8_t byte_data )
{
  if ( address == 0x4100 )
  {
    Map243_Regs[0] = byte_data;
  }
  else if ( address == 0x4101 )
  {
    switch ( Map243_Regs[0] & 0x07 )
    {
      case 0x02:
        Map243_Regs[1] = byte_data & 0x01;
        break;

      case 0x00:
      case 0x04:
      case 0x07:
        Map243_Regs[2] = ( byte_data & 0x01 ) << 1;
        break;

      /* Set ROM Banks  设置 ROM 银行 */
      case 0x05:
        ROMBANK0 = ROMPAGE( ( byte_data * 4 + 0 ) % ( NesHeader.rom_size << 1 ) );
        ROMBANK1 = ROMPAGE( ( byte_data * 4 + 1 ) % ( NesHeader.rom_size << 1 ) );
        ROMBANK2 = ROMPAGE( ( byte_data * 4 + 2 ) % ( NesHeader.rom_size << 1 ) );
        ROMBANK3 = ROMPAGE( ( byte_data * 4 + 3 ) % ( NesHeader.rom_size << 1 ) );
        break;

      case 0x06:
        Map243_Regs[3] = ( byte_data & 0x03 ) << 2;
        break;
    }

    /* Set PPU Banks  设置 PPU 银行 */
    if ( ( NesHeader.v_rom_size << 3 ) <= 64 )
    {
      uint8_t chr_bank = ( Map243_Regs[2] + Map243_Regs[3] ) >> 1;

      PPUBANK[ 0 ] = VROMPAGE( ( chr_bank * 8 + 0 ) % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 1 ] = VROMPAGE( ( chr_bank * 8 + 1 ) % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 2 ] = VROMPAGE( ( chr_bank * 8 + 2 ) % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 3 ] = VROMPAGE( ( chr_bank * 8 + 3 ) % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 4 ] = VROMPAGE( ( chr_bank * 8 + 4 ) % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 5 ] = VROMPAGE( ( chr_bank * 8 + 5 ) % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 6 ] = VROMPAGE( ( chr_bank * 8 + 6 ) % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 7 ] = VROMPAGE( ( chr_bank * 8 + 7 ) % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();
    }
    else
    {
      uint8_t chr_bank = Map243_Regs[1] + Map243_Regs[2] + Map243_Regs[3];

      PPUBANK[ 0 ] = VROMPAGE( ( chr_bank * 8 + 0 ) % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 1 ] = VROMPAGE( ( chr_bank * 8 + 1 ) % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 2 ] = VROMPAGE( ( chr_bank * 8 + 2 ) % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 3 ] = VROMPAGE( ( chr_bank * 8 + 3 ) % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 4 ] = VROMPAGE( ( chr_bank * 8 + 4 ) % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 5 ] = VROMPAGE( ( chr_bank * 8 + 5 ) % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 6 ] = VROMPAGE( ( chr_bank * 8 + 6 ) % ( NesHeader.v_rom_size << 3 ) );
      PPUBANK[ 7 ] = VROMPAGE( ( chr_bank * 8 + 7 ) % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();
    }
  }
}
