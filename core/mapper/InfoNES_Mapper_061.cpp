/*===================================================================*/
/*                                                                   */
/*                            Mapper 61  映射器 61                   */
/*                                                                   */
/*===================================================================*/

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 61  初始化映射器 61                            */
/*-------------------------------------------------------------------*/
void Map61_Init()
{
  /* Initialize Mapper  初始化映射器 */
  MapperInit = Map61_Init;

  /* Write to Mapper  写入映射器 */
  MapperWrite = Map61_Write;

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
  ROMBANK2 = ROMLASTPAGE( 1 );
  ROMBANK3 = ROMLASTPAGE( 0 );

  /* Set up wiring of the interrupt pin  设置中断引脚接线方式 */
  K6502_Set_Int_Wiring( 1, 1 ); 
}

/*-------------------------------------------------------------------*/
/*  Mapper 61 Write Function  映射器 61：写函数                      */
/*-------------------------------------------------------------------*/
void Map61_Write( uint16_t address, uint8_t byte_data )
{
	uint8_t bank_number;

	switch( address & 0x30 ) {
		case	0x00:
		case	0x30:
			ROMBANK0 = ROMPAGE((((address&0x0F)<<2)+0) % (NesHeader.rom_size<<1));
			ROMBANK1 = ROMPAGE((((address&0x0F)<<2)+1) % (NesHeader.rom_size<<1));
			ROMBANK2 = ROMPAGE((((address&0x0F)<<2)+2) % (NesHeader.rom_size<<1));
			ROMBANK3 = ROMPAGE((((address&0x0F)<<2)+3) % (NesHeader.rom_size<<1));
			break;
		case	0x10:
		case	0x20:
			bank_number = ((address & 0x0F)<<1)|((address&0x20)>>4);

			ROMBANK0 = ROMPAGE(((bank_number<<1)+0) % (NesHeader.rom_size<<1));
			ROMBANK1 = ROMPAGE(((bank_number<<1)+1) % (NesHeader.rom_size<<1));
			ROMBANK2 = ROMPAGE(((bank_number<<1)+0) % (NesHeader.rom_size<<1));
			ROMBANK3 = ROMPAGE(((bank_number<<1)+1) % (NesHeader.rom_size<<1));
			break;
	}

	if( address & 0x80 ) InfoNES_Mirroring( 0 );
	else		   InfoNES_Mirroring( 1 );
}
