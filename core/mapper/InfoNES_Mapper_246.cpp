/*===================================================================*/
/*                                                                   */
/*                 Mapper 246 : Phone Serm Berm  映射器 246：Phone Serm Berm（拼音标题） */
/*                                                                   */
/*===================================================================*/

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 246  初始化映射器 246                          */
/*-------------------------------------------------------------------*/
void Map246_Init()
{
  /* Initialize Mapper  初始化映射器 */
  MapperInit = Map246_Init;

  /* Write to Mapper  写入映射器 */
  MapperWrite = Map0_Write;

  /* Write to SRAM  写入 SRAM */
  MapperSram = Map246_Sram;

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
/*  Mapper 246 Write to SRAM Function  映射器 246：写 SRAM 函数      */
/*-------------------------------------------------------------------*/
void Map246_Sram( uint16_t address, uint8_t byte_data )
{
  if( address>=0x6000 && address<0x8000 ) {
    switch( address ) {
    case 0x6000:
      ROMBANK0 = ROMPAGE(((byte_data<<2)+0) % (NesHeader.rom_size<<1));
      break;
    case 0x6001:
      ROMBANK1 = ROMPAGE(((byte_data<<2)+1) % (NesHeader.rom_size<<1));
      break;
    case 0x6002:
      ROMBANK2 = ROMPAGE(((byte_data<<2)+2) % (NesHeader.rom_size<<1));
      break;
    case 0x6003: 
      ROMBANK3 = ROMPAGE(((byte_data<<2)+3) % (NesHeader.rom_size<<1));
      break;
    case 0x6004:
      PPUBANK[ 0 ] = VROMPAGE(((byte_data<<1)+0) % (NesHeader.v_rom_size<<3)); 
      PPUBANK[ 1 ] = VROMPAGE(((byte_data<<1)+1) % (NesHeader.v_rom_size<<3)); 
      InfoNES_SetupChr();
      break;
    case 0x6005:
      PPUBANK[ 2 ] = VROMPAGE(((byte_data<<1)+0) % (NesHeader.v_rom_size<<3)); 
      PPUBANK[ 3 ] = VROMPAGE(((byte_data<<1)+1) % (NesHeader.v_rom_size<<3)); 
      InfoNES_SetupChr();
      break;
    case 0x6006:
      PPUBANK[ 4 ] = VROMPAGE(((byte_data<<1)+0) % (NesHeader.v_rom_size<<3)); 
      PPUBANK[ 5 ] = VROMPAGE(((byte_data<<1)+1) % (NesHeader.v_rom_size<<3)); 
      InfoNES_SetupChr();
      break;
    case 0x6007:
      PPUBANK[ 6 ] = VROMPAGE(((byte_data<<1)+0) % (NesHeader.v_rom_size<<3)); 
      PPUBANK[ 7 ] = VROMPAGE(((byte_data<<1)+1) % (NesHeader.v_rom_size<<3)); 
      InfoNES_SetupChr();
      break;
    default:
      SRAMBANK[address&0x1FFF] = byte_data;
      break;
    }
  }
}
