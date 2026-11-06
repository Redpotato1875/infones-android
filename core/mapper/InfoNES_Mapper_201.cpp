/*===================================================================*/
/*                                                                   */
/*                   Mapper 201 (21-in-1)  映射器 201（21-in-1）     */
/*                                                                   */
/*===================================================================*/

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 201  初始化映射器 201                          */
/*-------------------------------------------------------------------*/
void Map201_Init()
{
  /* Initialize Mapper  初始化映射器 */
  MapperInit = Map201_Init;

  /* Write to Mapper  写入映射器 */
  MapperWrite = Map201_Write;

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
  ROMBANK0 = ROMPAGE( 0 );
  ROMBANK1 = ROMPAGE( 1 );
  ROMBANK2 = ROMPAGE( 0 );
  ROMBANK3 = ROMPAGE( 1 );

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
/*  Mapper 201 Write Function  映射器 201：写函数                    */
/*-------------------------------------------------------------------*/
void Map201_Write( uint16_t address, uint8_t byte_data )
{
	uint8_t bank_number = (uint8_t)address & 0x03;
	if (!(address&0x08) )
		bank_number = 0;

	/* Set ROM Banks  设置 ROM 银行 */
  ROMBANK0 = ROMPAGE(((bank_number<<2)+0) % (NesHeader.rom_size<<1));
  ROMBANK1 = ROMPAGE(((bank_number<<2)+1) % (NesHeader.rom_size<<1));
  ROMBANK2 = ROMPAGE(((bank_number<<2)+2) % (NesHeader.rom_size<<1));
  ROMBANK3 = ROMPAGE(((bank_number<<2)+3) % (NesHeader.rom_size<<1));

	/* Set PPU Banks  设置 PPU 银行 */
	PPUBANK[0] = VROMPAGE(((bank_number<<3)+0) % (NesHeader.v_rom_size<<3));
	PPUBANK[1] = VROMPAGE(((bank_number<<3)+1) % (NesHeader.v_rom_size<<3));
	PPUBANK[2] = VROMPAGE(((bank_number<<3)+2) % (NesHeader.v_rom_size<<3));
	PPUBANK[3] = VROMPAGE(((bank_number<<3)+3) % (NesHeader.v_rom_size<<3));
	PPUBANK[4] = VROMPAGE(((bank_number<<3)+4) % (NesHeader.v_rom_size<<3));
	PPUBANK[5] = VROMPAGE(((bank_number<<3)+5) % (NesHeader.v_rom_size<<3));
	PPUBANK[6] = VROMPAGE(((bank_number<<3)+6) % (NesHeader.v_rom_size<<3));
	PPUBANK[7] = VROMPAGE(((bank_number<<3)+7) % (NesHeader.v_rom_size<<3));
	InfoNES_SetupChr();
}

