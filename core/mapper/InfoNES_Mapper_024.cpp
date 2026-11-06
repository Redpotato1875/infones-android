/*===================================================================*/
/*                                                                   */
/*                  Mapper 24 (Konami VRC6)  映射器 24（Konami VRC6） */
/*                                                                   */
/*===================================================================*/

uint8_t Map24_IRQ_Count;
uint8_t Map24_IRQ_State;
uint8_t Map24_IRQ_Latch;

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 24  初始化映射器 24                            */
/*-------------------------------------------------------------------*/
void Map24_Init()
{
  /* Initialize Mapper  初始化映射器 */
  MapperInit = Map24_Init;

  /* Write to Mapper  写入映射器 */
  MapperWrite = Map24_Write;

  /* Write to SRAM  写入 SRAM */
  MapperSram = Map0_Sram;

  /* Write to APU  写入 APU */
  MapperApu = Map0_Apu;

  /* Read from APU  从 APU 读取 */
  MapperReadApu = Map0_ReadApu;

  /* Callback at VSync  场同步回调 */
  MapperVSync = Map0_VSync;

  /* Callback at HSync  行同步回调 */
  MapperHSync = Map24_HSync;

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
/*  Mapper 24 Write Function  映射器 24：写函数                      */
/*-------------------------------------------------------------------*/
void Map24_Write( uint16_t address, uint8_t byte_data )
{
  switch ( address )
  {
    case 0x8000:
      /* Set ROM Banks  设置 ROM 银行 */
      ROMBANK0 = ROMPAGE( ( byte_data + 0 ) % ( NesHeader.rom_size << 1) );
      ROMBANK1 = ROMPAGE( ( byte_data + 1 ) % ( NesHeader.rom_size << 1) );
      break;

    case 0xb003:
      /* Name Table Mirroring  命名表镜像 */
      switch ( byte_data & 0x0c )
      {
        case 0x00:
          InfoNES_Mirroring( 1 );   /* Vertical  垂直 */
          break;
        case 0x04:
          InfoNES_Mirroring( 0 );   /* Horizontal  水平 */
          break;
        case 0x08:
          InfoNES_Mirroring( 3 );   /* One Screen 0x2000  单屏 0x2000 */
          break;
        case 0x0c:
          InfoNES_Mirroring( 2 );   /* One Screen 0x2400  单屏 0x2400 */
          break;
      }
      break;

	  case 0xC000:
      ROMBANK2 = ROMPAGE( byte_data % ( NesHeader.rom_size << 1) );
		  break;

	  case 0xD000:
      PPUBANK[ 0 ] = VROMPAGE( byte_data % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();
      break;

	  case 0xD001:
      PPUBANK[ 1 ] = VROMPAGE( byte_data % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();
      break;

	  case 0xD002:
      PPUBANK[ 2 ] = VROMPAGE( byte_data % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();
      break;

	  case 0xD003:
      PPUBANK[ 3 ] = VROMPAGE( byte_data % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();
      break;

	  case 0xE000:
      PPUBANK[ 4 ] = VROMPAGE( byte_data % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();
      break;

	  case 0xE001:
      PPUBANK[ 5 ] = VROMPAGE( byte_data % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();
      break;

	  case 0xE002:
      PPUBANK[ 6 ] = VROMPAGE( byte_data % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();
      break;

	  case 0xE003:
      PPUBANK[ 7 ] = VROMPAGE( byte_data % ( NesHeader.v_rom_size << 3 ) );
      InfoNES_SetupChr();
      break;

	  case 0xF000:
			Map24_IRQ_Latch = byte_data;
	  	break;

	  case 0xF001:
			Map24_IRQ_State = byte_data & 0x03;
			if(Map24_IRQ_State & 0x02)
			{
				Map24_IRQ_Count = Map24_IRQ_Latch;
			}
		  break;

	  case 0xF002:
			if(Map24_IRQ_State & 0x01)
			{
				Map24_IRQ_State |= 0x02;
			}
			else
			{
				Map24_IRQ_State &= 0x01;
			}
		break;
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 24 H-Sync Function  映射器 24：行同步函数                  */
/*-------------------------------------------------------------------*/
void Map24_HSync()
{
/*
 *  Callback at HSync  行同步回调
 *
 */
	if(Map24_IRQ_State & 0x02)
	{
	  if(Map24_IRQ_Count == 0xFF)
		{
			IRQ_REQ;
			Map24_IRQ_Count = Map24_IRQ_Latch;
		}
		else
		{
			Map24_IRQ_Count++;
		}
	}
}
