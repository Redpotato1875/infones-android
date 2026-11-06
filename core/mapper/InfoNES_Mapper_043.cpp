/*===================================================================*/
/*                                                                   */
/*                         Mapper 43 (SMB2J)  映射器 43（SMB2J）     */
/*                                                                   */
/*===================================================================*/

uint32_t Map43_IRQ_Cnt;
uint8_t Map43_IRQ_Enable;

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 43  初始化映射器 43                            */
/*-------------------------------------------------------------------*/
void Map43_Init()
{
  /* Initialize Mapper  初始化映射器 */
  MapperInit = Map43_Init;

  /* Write to Mapper  写入映射器 */
  MapperWrite = Map43_Write;

  /* Write to SRAM  写入 SRAM */
  MapperSram = Map0_Sram;

  /* Write to APU  写入 APU */
  MapperApu = Map43_Apu;

  /* Read from APU  从 APU 读取 */
  MapperReadApu = Map43_ReadApu;

  /* Callback at VSync  场同步回调 */
  MapperVSync = Map0_VSync;

  /* Callback at HSync  行同步回调 */
  MapperHSync = Map43_HSync;

  /* Callback at PPU  PPU 回调 */
  MapperPPU = Map0_PPU;

  /* Callback at Rendering Screen ( 1:BG, 0:Sprite )  渲染画面回调（1：背景，0：精灵） */
  MapperRenderScreen = Map0_RenderScreen;

  /* Set SRAM Banks  设置 SRAM 银行 */
  SRAMBANK = ROMPAGE( 2 );

  /* Set ROM Banks  设置 ROM 银行 */
  ROMBANK0 = ROMPAGE( 1 );
  ROMBANK1 = ROMPAGE( 0 );
  ROMBANK2 = ROMPAGE( 4 );
  ROMBANK3 = ROMPAGE( 9 );

  /* Initialize State Registers  初始化状态寄存器 */
	Map43_IRQ_Enable = 1;
	Map43_IRQ_Cnt = 0;

  /* Set PPU Banks  设置 PPU 银行 */
  if ( NesHeader.v_rom_size > 0 )
  {
    for ( int page_number = 0; page_number < 8; ++page_number )
      PPUBANK[ page_number ] = VROMPAGE( page_number );
    InfoNES_SetupChr();
  }

  /* Set up wiring of the interrupt pin  设置中断引脚接线方式 */
  K6502_Set_Int_Wiring( 1, 1 ); 
}

/*-------------------------------------------------------------------*/
/*  Mapper 43 Read from APU Function  映射器 43：读 APU 函数         */
/*-------------------------------------------------------------------*/
uint8_t Map43_ReadApu( uint16_t address )
{
  if ( 0x5000 <= address && address < 0x6000 ) 
  {
    return ROM[ 0x2000*8 + 0x1000 + (address - 0x5000) ];
  }
  return (uint8_t)(address >> 8);
}

/*-------------------------------------------------------------------*/
/*  Mapper 43 Write to APU Function  映射器 43：写 APU 函数          */
/*-------------------------------------------------------------------*/
void Map43_Apu( uint16_t address, uint8_t byte_data )
{
	if( (address&0xF0FF) == 0x4022 ) 
  {
		switch( byte_data&0x07 ) 
    {
			case	0x00:
			case	0x02:
			case	0x03:
			case	0x04:
        ROMBANK2 = ROMPAGE( 4 );
				break;
			case	0x01:
        ROMBANK2 = ROMPAGE( 3 );
				break;
			case	0x05:
        ROMBANK2 = ROMPAGE( 7 );
				break;
			case	0x06:
        ROMBANK2 = ROMPAGE( 5 );
				break;
			case	0x07:
        ROMBANK2 = ROMPAGE( 6 );
				break;
		}
	}
}

/*-------------------------------------------------------------------*/
/*  Mapper 43 Write Function  映射器 43：写函数                      */
/*-------------------------------------------------------------------*/
void Map43_Write( uint16_t address, uint8_t byte_data )
{
	if( address == 0x8122 ) {
		if( byte_data&0x03 ) {
			Map43_IRQ_Enable = 1;
		} else {
			Map43_IRQ_Cnt = 0;
			Map43_IRQ_Enable = 0;
		}
	}
}

/*-------------------------------------------------------------------*/
/*  Mapper 43 H-Sync Function  映射器 43：行同步函数                 */
/*-------------------------------------------------------------------*/
void Map43_HSync()
{
	if( Map43_IRQ_Enable ) 
  {
		Map43_IRQ_Cnt += 114;
		if( Map43_IRQ_Cnt >= 4096 ) {
			Map43_IRQ_Cnt -= 4096;
			IRQ_REQ;
		}
	}
}
