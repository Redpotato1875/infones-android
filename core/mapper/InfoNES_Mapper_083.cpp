/*===================================================================*/
/*                                                                   */
/*                        Mapper 83 (Pirates)  映射器 83（Pirates）  */
/*                                                                   */
/*===================================================================*/

uint8_t Map83_Regs[3];
uint32_t Map83_Chr_Bank;
uint32_t Map83_IRQ_Cnt;
uint8_t Map83_IRQ_Enabled;

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 83  初始化映射器 83                            */
/*-------------------------------------------------------------------*/
void Map83_Init()
{
  /* Initialize Mapper  初始化映射器 */
  MapperInit = Map83_Init;

  /* Write to Mapper  写入映射器 */
  MapperWrite = Map83_Write;

  /* Write to SRAM  写入 SRAM */
  MapperSram = Map0_Sram;

  /* Write to APU  写入 APU */
  MapperApu = Map83_Apu;

  /* Read from APU  从 APU 读取 */
  MapperReadApu = Map83_ReadApu;

  /* Callback at VSync  场同步回调 */
  MapperVSync = Map0_VSync;

  /* Callback at HSync  行同步回调 */
  MapperHSync = Map83_HSync;

  /* Callback at PPU  PPU 回调 */
  MapperPPU = Map0_PPU;

  /* Callback at Rendering Screen ( 1:BG, 0:Sprite )  渲染画面回调（1：背景，0：精灵） */
  MapperRenderScreen = Map0_RenderScreen;

  /* Set SRAM Banks  设置 SRAM 银行 */
  SRAMBANK = SRAM;

  /* Initialize Registers  初始化寄存器 */
  Map83_Regs[0] = Map83_Regs[1] = Map83_Regs[2] = 0;

  /* Set ROM Banks  设置 ROM 银行 */
  if ( ( NesHeader.rom_size << 1 ) >= 32 )
  {
    ROMBANK0 = ROMPAGE( 0 );
    ROMBANK1 = ROMPAGE( 1 );
    ROMBANK2 = ROMPAGE( 30 );
    ROMBANK3 = ROMPAGE( 31 );
    Map83_Regs[ 1 ] = 0x30;
  }
  else
  {
    ROMBANK0 = ROMPAGE( 0 );
    ROMBANK1 = ROMPAGE( 1 );
    ROMBANK2 = ROMLASTPAGE( 1 );
    ROMBANK3 = ROMLASTPAGE( 0 );
  }

  /* Set PPU Banks  设置 PPU 银行 */
  if ( NesHeader.v_rom_size > 0 )
  {
    for ( int page_number = 0; page_number < 8; ++page_number )
      PPUBANK[ page_number ] = VROMPAGE( page_number );
    InfoNES_SetupChr();
  }

  /* Initialize IRQ Registers  初始化 IRQ 寄存器 */
  Map83_IRQ_Enabled = 0;
  Map83_IRQ_Cnt = 0;
  Map83_Chr_Bank = 0;

  /* Set up wiring of the interrupt pin  设置中断引脚接线方式 */
  K6502_Set_Int_Wiring( 1, 1 ); 
}

/*-------------------------------------------------------------------*/
/*  Mapper 83 Read from APU Function  映射器 83：读 APU 函数         */
/*-------------------------------------------------------------------*/
uint8_t Map83_ReadApu( uint16_t address )
{
  if ( ( address & 0x5100 ) == 0x5100 )
  {
    return Map83_Regs[2];
  }
  else
  {
    return address >> 8;
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 83 Write to APU Function  映射器 83：写 APU 函数          */
/*-------------------------------------------------------------------*/
void Map83_Apu( uint16_t address, uint8_t byte_data )
{
  switch(address)
  {
    case 0x5101:
    case 0x5102:
    case 0x5103:
      Map83_Regs[2] = byte_data;
      break;
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 83 Write Function  映射器 83：写函数                      */
/*-------------------------------------------------------------------*/
void Map83_Write( uint16_t address, uint8_t byte_data )
{
  /* Set ROM Banks  设置 ROM 银行 */
  switch( address )
  {
    case 0x8000:
    case 0xB000:
    case 0xB0FF:
    case 0xB1FF:
      Map83_Regs[0] = byte_data;
      Map83_Chr_Bank = (byte_data & 0x30) << 4;

      ROMBANK0 = ROMPAGE( (byte_data*2+0) % (NesHeader.rom_size << 1) );
      ROMBANK1 = ROMPAGE( (byte_data*2+1) % (NesHeader.rom_size << 1) );
      ROMBANK2 = ROMPAGE( (((byte_data&0x30)|0x0F)*2+0) % (NesHeader.rom_size << 1) );
      ROMBANK3 = ROMPAGE( (((byte_data&0x30)|0x0F)*2+1) % (NesHeader.rom_size << 1) );
      break;

    case 0x8100:
      if ( NesHeader.v_rom_size <= 32 )
      {
	Map83_Regs[1] = byte_data;
      }
      if ((byte_data & 0x03) == 0x00)
      {
	InfoNES_Mirroring( 1 );
      }
      else if((byte_data & 0x03) == 0x01)
      {
	InfoNES_Mirroring( 0 );
      }
      else if((byte_data & 0x03) == 0x02)
      {
	InfoNES_Mirroring( 3 );
      }
      else
      {
	InfoNES_Mirroring( 2 );
      }
      break;

    case 0x8200:
      Map83_IRQ_Cnt = ( Map83_IRQ_Cnt & 0xFF00 ) | (uint32_t)byte_data;
      break;

    case 0x8201:
      Map83_IRQ_Cnt = ( Map83_IRQ_Cnt & 0x00FF ) | ((uint32_t)byte_data << 8);
      Map83_IRQ_Enabled = byte_data;
      break;

    case 0x8300:
      ROMBANK0 = ROMPAGE( byte_data % (NesHeader.rom_size << 1) );	
      break;

    case 0x8301:
      ROMBANK1 = ROMPAGE( byte_data % (NesHeader.rom_size << 1) );	
      break;

    case 0x8302:
      ROMBANK2 = ROMPAGE( byte_data % (NesHeader.rom_size << 1) );	
      break;

    case 0x8310:
      if ((Map83_Regs[1] & 0x30) == 0x30)
      {
	PPUBANK[ 0 ] = VROMPAGE( (Map83_Chr_Bank^byte_data) % (NesHeader.v_rom_size << 3) );
	InfoNES_SetupChr();
      }
      else if((Map83_Regs[1] & 0x30) == 0x10 || (Map83_Regs[1] & 0x30) == 0x20)
      { 
	PPUBANK[ 0 ] = VROMPAGE( (((Map83_Chr_Bank^byte_data)*2)+0) % (NesHeader.v_rom_size << 3) );
	PPUBANK[ 1 ] = VROMPAGE( (((Map83_Chr_Bank^byte_data)*2)+1) % (NesHeader.v_rom_size << 3) );
	InfoNES_SetupChr();
      }
      break;

    case 0x8311:
      if ((Map83_Regs[1] & 0x30) == 0x30)
      {
	PPUBANK[ 1 ] = VROMPAGE( (Map83_Chr_Bank^byte_data) % (NesHeader.v_rom_size << 3) );
	InfoNES_SetupChr();
      }
      else if((Map83_Regs[1] & 0x30) == 0x10 || (Map83_Regs[1] & 0x30) == 0x20)
      {
	PPUBANK[ 2 ] = VROMPAGE( (((Map83_Chr_Bank^byte_data)*2)+0) % (NesHeader.v_rom_size << 3) );
	PPUBANK[ 3 ] = VROMPAGE( (((Map83_Chr_Bank^byte_data)*2)+1) % (NesHeader.v_rom_size << 3) );
	InfoNES_SetupChr();
      }
      break;

    case 0x8312:
      if ((Map83_Regs[1] & 0x30) == 0x30)
      {
	PPUBANK[ 2 ] = VROMPAGE( (Map83_Chr_Bank^byte_data) % (NesHeader.v_rom_size << 3) );
	InfoNES_SetupChr();
      }
      else if((Map83_Regs[1] & 0x30) == 0x10 || (Map83_Regs[1] & 0x30) == 0x20)
      {
	PPUBANK[ 4 ] = VROMPAGE( (((Map83_Chr_Bank^byte_data)*2)+0) % (NesHeader.v_rom_size << 3) );
	PPUBANK[ 5 ] = VROMPAGE( (((Map83_Chr_Bank^byte_data)*2)+1) % (NesHeader.v_rom_size << 3) );
	InfoNES_SetupChr();
      }
      break;

    case 0x8313:
      if ((Map83_Regs[1] & 0x30) == 0x30)
      {
	PPUBANK[ 3 ] = VROMPAGE( (Map83_Chr_Bank^byte_data) % (NesHeader.v_rom_size << 3) );
	InfoNES_SetupChr();
      }
      else if((Map83_Regs[1] & 0x30) == 0x10 || (Map83_Regs[1] & 0x30) == 0x20)
      {
	PPUBANK[ 6 ] = VROMPAGE( (((Map83_Chr_Bank^byte_data)*2)+0) % (NesHeader.v_rom_size << 3) );
	PPUBANK[ 7 ] = VROMPAGE( (((Map83_Chr_Bank^byte_data)*2)+1) % (NesHeader.v_rom_size << 3) );
	InfoNES_SetupChr();
      }
      break;

    case 0x8314:
      if ((Map83_Regs[1] & 0x30) == 0x30)
      { 
	PPUBANK[ 4 ] = VROMPAGE( (Map83_Chr_Bank^byte_data) % (NesHeader.v_rom_size << 3) );
	InfoNES_SetupChr();
      }
      else if((Map83_Regs[1] & 0x30) == 0x10 || (Map83_Regs[1] & 0x30) == 0x20)
      {
	PPUBANK[ 4 ] = VROMPAGE( (((Map83_Chr_Bank^byte_data)*2)+0) % (NesHeader.v_rom_size << 3) );
	PPUBANK[ 5 ] = VROMPAGE( (((Map83_Chr_Bank^byte_data)*2)+1) % (NesHeader.v_rom_size << 3) );
	InfoNES_SetupChr();
      }
      break;

    case 0x8315:
      if ((Map83_Regs[1] & 0x30) == 0x30)
      {
	PPUBANK[ 5 ] = VROMPAGE( (Map83_Chr_Bank^byte_data) % (NesHeader.v_rom_size << 3) );
	InfoNES_SetupChr();
      }
      else if ((Map83_Regs[1] & 0x30) == 0x10 || (Map83_Regs[1] & 0x30) == 0x20)
      {
	PPUBANK[ 6 ] = VROMPAGE( (((Map83_Chr_Bank^byte_data)*2)+0) % (NesHeader.v_rom_size << 3) );
	PPUBANK[ 7 ] = VROMPAGE( (((Map83_Chr_Bank^byte_data)*2)+1) % (NesHeader.v_rom_size << 3) );
	InfoNES_SetupChr();
      }
      break;

    case 0x8316:
      if ((Map83_Regs[1] & 0x30) == 0x30)
      {
	PPUBANK[ 6 ] = VROMPAGE( (Map83_Chr_Bank^byte_data) % (NesHeader.v_rom_size << 3) );
	InfoNES_SetupChr();
      }
      else if ((Map83_Regs[1] & 0x30) == 0x10 || (Map83_Regs[1] & 0x30) == 0x20)
      {
	PPUBANK[ 4 ] = VROMPAGE( (((Map83_Chr_Bank^byte_data)*2)+0) % (NesHeader.v_rom_size << 3) );
	PPUBANK[ 5 ] = VROMPAGE( (((Map83_Chr_Bank^byte_data)*2)+1) % (NesHeader.v_rom_size << 3) );
	InfoNES_SetupChr();
      }
      break;

    case 0x8317:
      if ((Map83_Regs[1] & 0x30) == 0x30)
      {
	PPUBANK[ 7 ] = VROMPAGE( (Map83_Chr_Bank^byte_data) % (NesHeader.v_rom_size << 3) );
	InfoNES_SetupChr();
      }
      else if ( (Map83_Regs[1] & 0x30) == 0x10 || (Map83_Regs[1] & 0x30) == 0x20)
      {
	PPUBANK[ 6 ] = VROMPAGE( (((Map83_Chr_Bank^byte_data)*2)+0) % (NesHeader.v_rom_size << 3) );
	PPUBANK[ 7 ] = VROMPAGE( (((Map83_Chr_Bank^byte_data)*2)+1) % (NesHeader.v_rom_size << 3) );
	InfoNES_SetupChr();
      }
      break;

    case 0x8318:
      ROMBANK0 = ROMPAGE( (((Map83_Regs[0]&0x30)|byte_data)*2+0) % (NesHeader.rom_size << 1) );	
      ROMBANK1 = ROMPAGE( (((Map83_Regs[0]&0x30)|byte_data)*2+1) % (NesHeader.rom_size << 1) );	
      break;
  }
}

/*-------------------------------------------------------------------*/
/*  Mapper 83 H-Sync Function  映射器 83：行同步函数                 */
/*-------------------------------------------------------------------*/
void Map83_HSync()
{
  if (Map83_IRQ_Enabled)
  {
    if (Map83_IRQ_Cnt <= 114)
    {
      IRQ_REQ;
      Map83_IRQ_Enabled = 0;
    }
    else
    {
      Map83_IRQ_Cnt -= 114;
    }
  }
}
