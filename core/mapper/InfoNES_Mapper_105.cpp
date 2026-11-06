/*===================================================================*/
/*                                                                   */
/*        Mapper 105 : Nintendo World Championship  映射器 105：任天堂世界锦标赛 */
/*                                                                   */
/*===================================================================*/

uint8_t	Map105_Init_State;
uint8_t	Map105_Write_Count;
uint8_t	Map105_Bits;
uint8_t	Map105_Reg[4];

uint8_t	Map105_IRQ_Enable;
int	Map105_IRQ_Counter;

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 105  初始化映射器 105                          */
/*-------------------------------------------------------------------*/
void Map105_Init()
{
  /* Initialize Mapper  初始化映射器 */
  MapperInit = Map105_Init;

  /* Write to Mapper  写入映射器 */
  MapperWrite = Map105_Write;

  /* Write to SRAM  写入 SRAM */
  MapperSram = Map0_Sram;

  /* Write to APU  写入 APU */
  MapperApu = Map0_Apu;

  /* Read from APU  从 APU 读取 */
  MapperReadApu = Map0_ReadApu;

  /* Callback at VSync  场同步回调 */
  MapperVSync = Map0_VSync;

  /* Callback at HSync  行同步回调 */
  MapperHSync = Map105_HSync;

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
  Map105_Reg[0] = 0x0C;
  Map105_Reg[1] = 0x00;
  Map105_Reg[2] = 0x00;
  Map105_Reg[3] = 0x10;
  
  Map105_Bits = 0;
  Map105_Write_Count = 0;
  
  Map105_IRQ_Counter = 0;
  Map105_IRQ_Enable = 0;
  Map105_Init_State = 0;

  /* Set up wiring of the interrupt pin  设置中断引脚接线方式 */
  K6502_Set_Int_Wiring( 1, 1 ); 
}

/*-------------------------------------------------------------------*/
/*  Mapper 105 Write Function  映射器 105：写函数                    */
/*-------------------------------------------------------------------*/
void Map105_Write( uint16_t address, uint8_t byte_data )
{
  uint16_t reg_num = (address & 0x7FFF) >> 13;

  if( byte_data & 0x80 ) {
    Map105_Bits = Map105_Write_Count = 0;
    if( reg_num == 0 ) {
      Map105_Reg[reg_num] |= 0x0C;
    }
  } else {
    Map105_Bits |= (byte_data & 1) << Map105_Write_Count++;
    if( Map105_Write_Count == 5) {
      Map105_Reg[reg_num] = Map105_Bits & 0x1F;
      Map105_Bits = Map105_Write_Count = 0;
    }
  }

  if( Map105_Reg[0] & 0x02 ) {
    if( Map105_Reg[0] & 0x01 ) {
      InfoNES_Mirroring( 0 );
    } else {
      InfoNES_Mirroring( 1 );
    }
  } else {
    if( Map105_Reg[0] & 0x01 ) {
      InfoNES_Mirroring( 3 );
    } else {
      InfoNES_Mirroring( 2 );
    }
  }
  
  switch( Map105_Init_State ) {
  case 0:
  case 1:
    Map105_Init_State++;
    break;

  case 2:
    if(Map105_Reg[1] & 0x08) {
      if (Map105_Reg[0] & 0x08) {
	if (Map105_Reg[0] & 0x04) {
	  ROMBANK0 = ROMPAGE( ((Map105_Reg[3] & 0x07) * 2 + 16) % ( NesHeader.rom_size << 1 ) );
	  ROMBANK1 = ROMPAGE( ((Map105_Reg[3] & 0x07) * 2 + 17) % ( NesHeader.rom_size << 1 ) );
	  ROMBANK2 = ROMPAGE( 30 % ( NesHeader.rom_size << 1 ) );
	  ROMBANK3 = ROMPAGE( 31 % ( NesHeader.rom_size << 1 ) );
	} else {
	  ROMBANK0 = ROMPAGE( 16 % ( NesHeader.rom_size << 1 ) );
	  ROMBANK1 = ROMPAGE( 17 % ( NesHeader.rom_size << 1 ) );
	  ROMBANK2 = ROMPAGE( ((Map105_Reg[3] & 0x07) * 2 + 16) % ( NesHeader.rom_size << 1 ) );
	  ROMBANK3 = ROMPAGE( ((Map105_Reg[3] & 0x07) * 2 + 17) % ( NesHeader.rom_size << 1 ) );
	}
      } else {
	ROMBANK0 = ROMPAGE( ((Map105_Reg[3] & 0x06) * 2 + 16) % ( NesHeader.rom_size << 1 ) );
	ROMBANK1 = ROMPAGE( ((Map105_Reg[3] & 0x06) * 2 + 17) % ( NesHeader.rom_size << 1 ) );
	ROMBANK2 = ROMPAGE( ((Map105_Reg[3] & 0x06) * 2 + 18) % ( NesHeader.rom_size << 1 ) );
	ROMBANK3 = ROMPAGE( ((Map105_Reg[3] & 0x06) * 2 + 19) % ( NesHeader.rom_size << 1 ) );
      }
    } else {
      ROMBANK0 = ROMPAGE( ((Map105_Reg[1] & 0x06) * 2 + 0) % ( NesHeader.rom_size << 1 ) );
      ROMBANK1 = ROMPAGE( ((Map105_Reg[1] & 0x06) * 2 + 1) % ( NesHeader.rom_size << 1 ) );
      ROMBANK2 = ROMPAGE( ((Map105_Reg[1] & 0x06) * 2 + 2) % ( NesHeader.rom_size << 1 ) );
      ROMBANK3 = ROMPAGE( ((Map105_Reg[1] & 0x06) * 2 + 3) % ( NesHeader.rom_size << 1 ) );
    }
    
    if( Map105_Reg[1] & 0x10 ) {
      Map105_IRQ_Counter = 0;
      Map105_IRQ_Enable = 0;
    } else {
      Map105_IRQ_Enable = 1;
    }
    break;

  default:
    break;
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 105 H-Sync Function  映射器 105：行同步函数               */
/*-------------------------------------------------------------------*/
void Map105_HSync()
{
  if( !PPU_Scanline ) {
    if( Map105_IRQ_Enable ) {
      Map105_IRQ_Counter += 29781;
    }
    if( ((Map105_IRQ_Counter | 0x21FFFFFF) & 0x3E000000) == 0x3E000000 ) {
      IRQ_REQ;
    }
  }
}

