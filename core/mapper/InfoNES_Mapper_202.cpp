/*===================================================================*/
/*                                                                   */
/*                   Mapper 202 (150-in-1)  映射器 202（150-in-1）   */
/*                                                                   */
/*===================================================================*/

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 202  初始化映射器 202                          */
/*-------------------------------------------------------------------*/
void Map202_Init()
{
  /* Initialize Mapper  初始化映射器 */
  MapperInit = Map202_Init;

  /* Write to Mapper  写入映射器 */
  MapperWrite = Map202_Write;

  /* Write to SRAM  写入 SRAM */
  MapperSram = Map0_Sram;

  /* Write to APU  写入 APU */
  MapperApu = Map202_Apu;

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
  ROMBANK0 = ROMPAGE( 12 );
  ROMBANK1 = ROMPAGE( 13 );
  ROMBANK2 = ROMPAGE( 14 );
  ROMBANK3 = ROMPAGE( 15 );

  /* Set PPU Banks  设置 PPU 银行 */
  if ( NesHeader.v_rom_size > 0 ) {
    for ( int page_number = 0; page_number < 8; ++page_number )
      PPUBANK[ page_number ] = VROMPAGE( page_number );
    InfoNES_SetupChr();
  }

  /* Set up wiring of the interrupt pin  设置中断引脚接线方式 */
  K6502_Set_Int_Wiring( 1, 1 ); 
}

/*-------------------------------------------------------------------*/
/*  Mapper 202 Write to APU Function  映射器 202：写 APU 函数        */
/*-------------------------------------------------------------------*/
void Map202_Apu( uint16_t address, uint8_t byte_data )
{
	Map202_WriteSub( address, byte_data );
}

/*-------------------------------------------------------------------*/
/*  Mapper 202 Write Function  映射器 202：写函数                    */
/*-------------------------------------------------------------------*/
void Map202_Write( uint16_t address, uint8_t byte_data )
{
	Map202_WriteSub( address, byte_data );
}

/*-------------------------------------------------------------------*/
/*  Mapper 202 Write Sub Function  映射器 202：写子函数              */
/*-------------------------------------------------------------------*/
void Map202_WriteSub( uint16_t address, uint8_t byte_data )
{
	int bank = (address>>1) & 0x07;

	/* Set ROM Banks  设置 ROM 银行 */
	ROMBANK0 = ROMPAGE(((bank<<1)+0) % (NesHeader.rom_size<<1));
	ROMBANK1 = ROMPAGE(((bank<<1)+1) % (NesHeader.rom_size<<1));

	if ((address&0x0c) == 0x0c) {
		ROMBANK2 = ROMPAGE((((bank+1)<<1)+0) % (NesHeader.rom_size<<1));
		ROMBANK3 = ROMPAGE((((bank+1)<<1)+1) % (NesHeader.rom_size<<1));
	} else {		
		ROMBANK2 = ROMPAGE((((bank+0)<<1)+0) % (NesHeader.rom_size<<1));
		ROMBANK3 = ROMPAGE((((bank+0)<<1)+1) % (NesHeader.rom_size<<1));
	}	

	/* Set PPU Banks  设置 PPU 银行 */
	PPUBANK[0] = VROMPAGE(((bank<<3)+0) % (NesHeader.v_rom_size<<3));
	PPUBANK[1] = VROMPAGE(((bank<<3)+1) % (NesHeader.v_rom_size<<3));
	PPUBANK[2] = VROMPAGE(((bank<<3)+2) % (NesHeader.v_rom_size<<3));
	PPUBANK[3] = VROMPAGE(((bank<<3)+3) % (NesHeader.v_rom_size<<3));
	PPUBANK[4] = VROMPAGE(((bank<<3)+4) % (NesHeader.v_rom_size<<3));
	PPUBANK[5] = VROMPAGE(((bank<<3)+5) % (NesHeader.v_rom_size<<3));
	PPUBANK[6] = VROMPAGE(((bank<<3)+6) % (NesHeader.v_rom_size<<3));
	PPUBANK[7] = VROMPAGE(((bank<<3)+7) % (NesHeader.v_rom_size<<3));
	InfoNES_SetupChr();

	if ( address & 0x01 ) {
		InfoNES_Mirroring( 0 );
	} else {
		InfoNES_Mirroring( 1 );
	}
}

