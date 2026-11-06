/*===================================================================*/
/*                                                                   */
/*                      Mapper 249 : MMC3                            */
/*                                                                   */
/*===================================================================*/

uint8_t	Map249_Spdata;
uint8_t	Map249_Reg[8];

uint8_t	Map249_IRQ_Enable;
uint8_t	Map249_IRQ_Counter;
uint8_t	Map249_IRQ_Latch;
uint8_t	Map249_IRQ_Request;

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 249                                            */
/*-------------------------------------------------------------------*/
void Map249_Init()
{
  /* Initialize Mapper */
  MapperInit = Map249_Init;

  /* Write to Mapper */
  MapperWrite = Map249_Write;

  /* Write to SRAM */
  MapperSram = Map0_Sram;

  /* Write to APU */
  MapperApu = Map249_Apu;

  /* Read from APU */
  MapperReadApu = Map0_ReadApu;

  /* Callback at VSync */
  MapperVSync = Map0_VSync;

  /* Callback at HSync */
  MapperHSync = Map249_HSync;

  /* Callback at PPU */
  MapperPPU = Map0_PPU;

  /* Callback at Rendering Screen ( 1:BG, 0:Sprite ) */
  MapperRenderScreen = Map0_RenderScreen;

  /* Set SRAM Banks */
  SRAMBANK = SRAM;

  /* Set Registers */
  for( int i = 0; i < 8; i++ ) {
    Map249_Reg[i] = 0x00;
  }

  /* Set ROM Banks */
  ROMBANK0 = ROMPAGE( 0 );
  ROMBANK1 = ROMPAGE( 1 );
  ROMBANK2 = ROMLASTPAGE( 1 );
  ROMBANK3 = ROMLASTPAGE( 0 );



  /* Set PPU Banks */
  if ( NesHeader.v_rom_size > 0 ) {
    for ( int page_number = 0; page_number < 8; ++page_number )
      PPUBANK[ page_number ] = VROMPAGE( page_number );
    InfoNES_SetupChr();
  }

  /* Set Registers */
  Map249_IRQ_Enable = 0;	// Disable
  Map249_IRQ_Counter = 0;
  Map249_IRQ_Latch = 0;
  Map249_IRQ_Request = 0;

  Map249_Spdata = 0;

  /* Set up wiring of the interrupt pin */
  K6502_Set_Int_Wiring( 1, 1 ); 
}

/*-------------------------------------------------------------------*/
/*  Mapper 249 Write Function                                        */
/*-------------------------------------------------------------------*/
void Map249_Write( uint16_t address, uint8_t byte_data )
{
  uint8_t	m0,m1,m2,m3,m4,m5,m6,m7;

  switch( address&0xFF01 ) {
  case	  0x8000:
  case    0x8800:
    Map249_Reg[0] = byte_data;
    break;
  case	  0x8001:
  case    0x8801:
    switch( Map249_Reg[0] & 0x07 ) {
    case	0x00:
      if( Map249_Spdata == 1 ) {
	m0=byte_data&0x1;
	m1=(byte_data&0x02)>>1;
	m2=(byte_data&0x04)>>2;
	m3=(byte_data&0x08)>>3;
	m4=(byte_data&0x10)>>4;
	m5=(byte_data&0x20)>>5;
	m6=(byte_data&0x40)>>6;
	m7=(byte_data&0x80)>>7;
	byte_data=(m5<<7)|(m4<<6)|(m2<<5)|(m6<<4)|(m7<<3)|(m3<<2)|(m1<<1)|m0;
      }
      PPUBANK[ 0 ] = VROMPAGE((byte_data&0xFE) % (NesHeader.v_rom_size<<3));
      PPUBANK[ 1 ] = VROMPAGE((byte_data|0x01) % (NesHeader.v_rom_size<<3));
      InfoNES_SetupChr();
      break;
    case	0x01:
      if( Map249_Spdata == 1 ) {
	m0=byte_data&0x1;
	m1=(byte_data&0x02)>>1;
	m2=(byte_data&0x04)>>2;
	m3=(byte_data&0x08)>>3;
	m4=(byte_data&0x10)>>4;
	m5=(byte_data&0x20)>>5;
	m6=(byte_data&0x40)>>6;
	m7=(byte_data&0x80)>>7;
	byte_data=(m5<<7)|(m4<<6)|(m2<<5)|(m6<<4)|(m7<<3)|(m3<<2)|(m1<<1)|m0;
      }
      PPUBANK[ 2 ] = VROMPAGE((byte_data&0xFE) % (NesHeader.v_rom_size<<3));
      PPUBANK[ 3 ] = VROMPAGE((byte_data|0x01) % (NesHeader.v_rom_size<<3));
      InfoNES_SetupChr();
      break;
    case	0x02:
      if( Map249_Spdata == 1 ) {
	m0=byte_data&0x1;
	m1=(byte_data&0x02)>>1;
	m2=(byte_data&0x04)>>2;
	m3=(byte_data&0x08)>>3;
	m4=(byte_data&0x10)>>4;
	m5=(byte_data&0x20)>>5;
	m6=(byte_data&0x40)>>6;
	m7=(byte_data&0x80)>>7;
	byte_data=(m5<<7)|(m4<<6)|(m2<<5)|(m6<<4)|(m7<<3)|(m3<<2)|(m1<<1)|m0;
      }
      PPUBANK[ 4 ] = VROMPAGE(byte_data % (NesHeader.v_rom_size<<3));
      InfoNES_SetupChr();
      break;
    case	0x03:
      if( Map249_Spdata == 1 ) {
	m0=byte_data&0x1;
	m1=(byte_data&0x02)>>1;
	m2=(byte_data&0x04)>>2;
	m3=(byte_data&0x08)>>3;
	m4=(byte_data&0x10)>>4;
	m5=(byte_data&0x20)>>5;
	m6=(byte_data&0x40)>>6;
	m7=(byte_data&0x80)>>7;
	byte_data=(m5<<7)|(m4<<6)|(m2<<5)|(m6<<4)|(m7<<3)|(m3<<2)|(m1<<1)|m0;
      }
      PPUBANK[ 5 ] = VROMPAGE(byte_data % (NesHeader.v_rom_size<<3));
      InfoNES_SetupChr();
      break;
    case	0x04:
      if( Map249_Spdata == 1 ) {
	m0=byte_data&0x1;
	m1=(byte_data&0x02)>>1;
	m2=(byte_data&0x04)>>2;
	m3=(byte_data&0x08)>>3;
	m4=(byte_data&0x10)>>4;
	m5=(byte_data&0x20)>>5;
	m6=(byte_data&0x40)>>6;
	m7=(byte_data&0x80)>>7;
	byte_data=(m5<<7)|(m4<<6)|(m2<<5)|(m6<<4)|(m7<<3)|(m3<<2)|(m1<<1)|m0;
      }
      PPUBANK[ 6 ] = VROMPAGE(byte_data % (NesHeader.v_rom_size<<3));
      InfoNES_SetupChr();
      break;
    case	0x05:
      if( Map249_Spdata == 1 ) {
	m0=byte_data&0x1;
	m1=(byte_data&0x02)>>1;
	m2=(byte_data&0x04)>>2;
	m3=(byte_data&0x08)>>3;
	m4=(byte_data&0x10)>>4;
	m5=(byte_data&0x20)>>5;
	m6=(byte_data&0x40)>>6;
	m7=(byte_data&0x80)>>7;
	byte_data=(m5<<7)|(m4<<6)|(m2<<5)|(m6<<4)|(m7<<3)|(m3<<2)|(m1<<1)|m0;
      }
      PPUBANK[ 7 ] = VROMPAGE(byte_data % (NesHeader.v_rom_size<<3));
      InfoNES_SetupChr();
      break;
    case	0x06:
      if( Map249_Spdata == 1 ) {
	if( byte_data < 0x20 ) {
	  m0=byte_data&0x1;
	  m1=(byte_data&0x02)>>1;
	  m2=(byte_data&0x04)>>2;
	  m3=(byte_data&0x08)>>3;
	  m4=(byte_data&0x10)>>4;
	  m5=0;
	  m6=0;
	  m7=0;
	  byte_data=(m7<<7)|(m6<<6)|(m5<<5)|(m2<<4)|(m1<<3)|(m3<<2)|(m4<<1)|m0;
	} else {
	  byte_data=byte_data-0x20;
	  m0=byte_data&0x1;
	  m1=(byte_data&0x02)>>1;
	  m2=(byte_data&0x04)>>2;
	  m3=(byte_data&0x08)>>3;
	  m4=(byte_data&0x10)>>4;
	  m5=(byte_data&0x20)>>5;
	  m6=(byte_data&0x40)>>6;
	  m7=(byte_data&0x80)>>7;
	  byte_data=(m5<<7)|(m4<<6)|(m2<<5)|(m6<<4)|(m7<<3)|(m3<<2)|(m1<<1)|m0;
	}
      }
      ROMBANK0 = ROMPAGE(byte_data % (NesHeader.rom_size<<1));
      break;
    case	0x07:
      if( Map249_Spdata == 1 ) {
	if( byte_data < 0x20 ) {
	  m0=byte_data&0x1;
	  m1=(byte_data&0x02)>>1;
	  m2=(byte_data&0x04)>>2;
	  m3=(byte_data&0x08)>>3;
	  m4=(byte_data&0x10)>>4;
	  m5=0;
	  m6=0;
	  m7=0;
	  byte_data=(m7<<7)|(m6<<6)|(m5<<5)|(m2<<4)|(m1<<3)|(m3<<2)|(m4<<1)|m0;
	} else {
	  byte_data=byte_data-0x20;
	  m0=byte_data&0x1;
	  m1=(byte_data&0x02)>>1;
	  m2=(byte_data&0x04)>>2;
	  m3=(byte_data&0x08)>>3;
	  m4=(byte_data&0x10)>>4;
	  m5=(byte_data&0x20)>>5;
	  m6=(byte_data&0x40)>>6;
	  m7=(byte_data&0x80)>>7;
	  byte_data=(m5<<7)|(m4<<6)|(m2<<5)|(m6<<4)|(m7<<3)|(m3<<2)|(m1<<1)|m0;
	}
      }
      ROMBANK1 = ROMPAGE(byte_data % (NesHeader.rom_size<<1));
      break;
    }
    break;
  case	  0xA000:
  case    0xA800:
    Map249_Reg[2] = byte_data;
    if( !ROM_FourScr ) {
      if( byte_data & 0x01 ) InfoNES_Mirroring( 0 );
      else		  InfoNES_Mirroring( 1 );
    }
    break;
  case	  0xA001:
  case    0xA801:
    Map249_Reg[3] = byte_data;
    break;
  case	  0xC000:
  case    0xC800:
    Map249_Reg[4] = byte_data;
    Map249_IRQ_Counter = byte_data;
    Map249_IRQ_Request = 0;
    break;
  case	  0xC001:
  case    0xC801:
    Map249_Reg[5] = byte_data;
    Map249_IRQ_Latch = byte_data;
    Map249_IRQ_Request = 0;
    break;
  case	  0xE000:
  case    0xE800:
    Map249_Reg[6] = byte_data;
    Map249_IRQ_Enable = 0;
    Map249_IRQ_Request = 0;
    break;
  case	  0xE001:
  case    0xE801:
    Map249_Reg[7] = byte_data;
    Map249_IRQ_Enable = 1;
    Map249_IRQ_Request = 0;
    break;
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 249 Write to APU Function                                 */
/*-------------------------------------------------------------------*/
void Map249_Apu( uint16_t address, uint8_t byte_data )
{
  if( address == 0x5000 ) {
    switch( byte_data ) {
    case	0x00:
      Map249_Spdata = 0;
      break;
    case	0x02:
      Map249_Spdata = 1;
      break;
    }
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 249 H-Sync Function                                       */
/*-------------------------------------------------------------------*/
void Map249_HSync()
{
  if( (PPU_Scanline >= 0 && PPU_Scanline <= 239) ) {
    if( PPU_R1 & R1_SHOW_SCR || PPU_R1 & R1_SHOW_SP ) {
      if( Map249_IRQ_Enable && !Map249_IRQ_Request ) {
	if( PPU_Scanline == 0 ) {
	  if( Map249_IRQ_Counter ) {
	    Map249_IRQ_Counter--;
	  }
	}
	if( !(Map249_IRQ_Counter--) ) {
	  Map249_IRQ_Request = 0xFF;
	  Map249_IRQ_Counter = Map249_IRQ_Latch;
	}
      }
    }
  }
  if( Map249_IRQ_Request ) {
    IRQ_REQ;
  }
}

