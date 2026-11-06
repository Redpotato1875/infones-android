/*===================================================================*/
/*                                                                   */
/*                            Mapper 1  映射器 1                     */
/*                                                                   */
/*===================================================================*/

uint8_t  Map1_Regs[ 4 ];
uint32_t Map1_Cnt;
uint8_t  Map1_Latch;
uint16_t  Map1_Last_Write_Addr;

enum Map1_Size_t
{
  Map1_SMALL,
  Map1_512K,
  Map1_1024K
};

Map1_Size_t Map1_Size;
uint32_t Map1_256K_base;
uint32_t Map1_swap;

// these are the 4 ROM banks currently selected  当前选中的 4 个 ROM 银行
uint32_t Map1_bank1;
uint32_t Map1_bank2;
uint32_t Map1_bank3;
uint32_t Map1_bank4;

uint32_t Map1_HI1;
uint32_t Map1_HI2;

/*-------------------------------------------------------------------*/
/*  Initialize Mapper 1  初始化映射器 1                              */
/*-------------------------------------------------------------------*/
void Map1_Init()
{
  uint32_t size_in_K;

  /* Initialize Mapper  初始化映射器 */
  MapperInit = Map1_Init;

  /* Write to Mapper  写入映射器 */
  MapperWrite = Map1_Write;

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

  /* Initialize State Registers  初始化状态寄存器 */
  Map1_Cnt = 0;
  Map1_Latch = 0x00;

  Map1_Regs[ 0 ] = 0x0c;
  Map1_Regs[ 1 ] = 0x00;
  Map1_Regs[ 2 ] = 0x00;
  Map1_Regs[ 3 ] = 0x00;

  size_in_K = ( NesHeader.rom_size << 1 ) * 8;

  if ( size_in_K == 1024 )
  {
    Map1_Size = Map1_1024K;
  } 
  else if(size_in_K == 512)
  {
    Map1_Size = Map1_512K;
  }
  else
  {
    Map1_Size = Map1_SMALL;
  }

  Map1_256K_base = 0; // use first 256K  使用第一个 256K
  Map1_swap = 0;

  if( Map1_Size == Map1_SMALL )
  {
    // set two high pages to last two banks  将高两位页设为最后两个银行
    Map1_HI1 = ( NesHeader.rom_size << 1 ) - 2;
    Map1_HI2 = ( NesHeader.rom_size << 1 ) - 1;
  }
  else
  {
    // set two high pages to last two banks of current 256K region  将高两位页设为当前 256K 区域的最后两个银行
    Map1_HI1 = ( 256 / 8 ) - 2;
    Map1_HI2 = ( 256 / 8 ) - 1;
  }

  // set CPU bank pointers  设置 CPU 银行指针
  Map1_bank1 = 0;
  Map1_bank2 = 1;
  Map1_bank3 = Map1_HI1;
  Map1_bank4 = Map1_HI2;

  /* Set ROM Banks  设置 ROM 银行 */
  Map1_set_ROM_banks();

  /* Set up wiring of the interrupt pin  设置中断引脚接线方式 */
  K6502_Set_Int_Wiring( 1, 1 );
}

void Map1_set_ROM_banks()
{
  ROMBANK0 = ROMPAGE( ( (Map1_256K_base << 5) + (Map1_bank1 & ((256/8)-1)) ) % ( NesHeader.rom_size << 1 ) );  
  ROMBANK1 = ROMPAGE( ( (Map1_256K_base << 5) + (Map1_bank2 & ((256/8)-1)) ) % ( NesHeader.rom_size << 1 ) );
  ROMBANK2 = ROMPAGE( ( (Map1_256K_base << 5) + (Map1_bank3 & ((256/8)-1)) ) % ( NesHeader.rom_size << 1 ) );
  ROMBANK3 = ROMPAGE( ( (Map1_256K_base << 5) + (Map1_bank4 & ((256/8)-1)) ) % ( NesHeader.rom_size << 1 ) ); 
}

/*-------------------------------------------------------------------*/
/*  Mapper 1 Write Function  映射器 1：写函数                        */
/*-------------------------------------------------------------------*/
void Map1_Write( uint16_t address, uint8_t byte_data )
{
/*
 * MMC1
 */
  uint32_t register_number;

  // if write is to a different reg, reset  若写入不同寄存器则复位
  if( ( address & 0x6000 ) != ( Map1_Last_Write_Addr & 0x6000 ) )
  {
    Map1_Cnt = 0;
    Map1_Latch = 0x00;
  }
  Map1_Last_Write_Addr = address;

  // if bit 7 set, reset and return  若 bit7 置位则复位并返回
  if ( byte_data & 0x80 )
  {
    Map1_Cnt = 0;
    Map1_Latch = 0x00;
    return;
  }

  if ( byte_data & 0x01 ) Map1_Latch |= ( 1 << Map1_Cnt );
  Map1_Cnt++;
  if( Map1_Cnt < 5 ) return;

  register_number = ( address & 0x7FFF ) >> 13;
  Map1_Regs[ register_number ] = Map1_Latch;

  Map1_Cnt = 0;
  Map1_Latch = 0x00;

  switch( register_number )
  {
    case 0:
      {
        // set mirroring  设置镜像
        if( Map1_Regs[0] & 0x02 )
        {
          if( Map1_Regs[0] & 0x01 )
          {
            InfoNES_Mirroring( 0 );
          }
          else
          {
            InfoNES_Mirroring( 1 );
          }
        }
        else
        {
          // one-screen mirroring  单屏镜像
          if( Map1_Regs[0] & 0x01 )
          {
            InfoNES_Mirroring( 2 );
          }
          else
          {
            InfoNES_Mirroring( 3 );
          }
        }
      }
      break;

    case 1:
      {
        uint8_t bank_num = Map1_Regs[1];

        if ( Map1_Size == Map1_1024K )
        {
          if ( Map1_Regs[0] & 0x10 )
          {
            if ( Map1_swap )
            {
              Map1_256K_base = (Map1_Regs[1] & 0x10) >> 4;
              if(Map1_Regs[0] & 0x08)
              {
                Map1_256K_base |= ((Map1_Regs[2] & 0x10) >> 3);
              }
              Map1_set_ROM_banks();
              Map1_swap = 0;
            }
            else
            {
              Map1_swap = 1;
            }
          }
          else
          {
            // use 1st or 4th 256K banks  使用第 1 或第 4 个 256K 银行
            Map1_256K_base = ( Map1_Regs[1] & 0x10 ) ? 3 : 0;
            Map1_set_ROM_banks();
          }
        }
        else if((Map1_Size == Map1_512K) && (!NesHeader.v_rom_size))
        {
          Map1_256K_base = (Map1_Regs[1] & 0x10) >> 4;
          Map1_set_ROM_banks();
        }
        else if ( NesHeader.v_rom_size )
        {
          // set VROM bank at $0000  设置 $0000 处的 VROM 银行
          if ( Map1_Regs[0] & 0x10 )
          {
            // swap 4K  交换 4K
            bank_num <<= 2;
            PPUBANK[ 0 ] = VROMPAGE( (bank_num+0) % (NesHeader.v_rom_size << 3) );
            PPUBANK[ 1 ] = VROMPAGE( (bank_num+1) % (NesHeader.v_rom_size << 3) );
            PPUBANK[ 2 ] = VROMPAGE( (bank_num+2) % (NesHeader.v_rom_size << 3) );
            PPUBANK[ 3 ] = VROMPAGE( (bank_num+3) % (NesHeader.v_rom_size << 3) );
            InfoNES_SetupChr();
          }
          else
          {
            // swap 8K  交换 8K
            bank_num <<= 2;
            PPUBANK[ 0 ] = VROMPAGE( (bank_num+0) % (NesHeader.v_rom_size << 3) );
            PPUBANK[ 1 ] = VROMPAGE( (bank_num+1) % (NesHeader.v_rom_size << 3) );
            PPUBANK[ 2 ] = VROMPAGE( (bank_num+2) % (NesHeader.v_rom_size << 3) );
            PPUBANK[ 3 ] = VROMPAGE( (bank_num+3) % (NesHeader.v_rom_size << 3) );
            PPUBANK[ 4 ] = VROMPAGE( (bank_num+4) % (NesHeader.v_rom_size << 3) );
            PPUBANK[ 5 ] = VROMPAGE( (bank_num+5) % (NesHeader.v_rom_size << 3) );
            PPUBANK[ 6 ] = VROMPAGE( (bank_num+6) % (NesHeader.v_rom_size << 3) );
            PPUBANK[ 7 ] = VROMPAGE( (bank_num+7) % (NesHeader.v_rom_size << 3) );
            InfoNES_SetupChr();
          }
        }
      }
      break;

    case 2:
      {
        uint8_t bank_num = Map1_Regs[2];

        if((Map1_Size == Map1_1024K) && (Map1_Regs[0] & 0x08))
        {
          if(Map1_swap)
          {
            Map1_256K_base =  (Map1_Regs[1] & 0x10) >> 4;
            Map1_256K_base |= ((Map1_Regs[2] & 0x10) >> 3);
            Map1_set_ROM_banks();
            Map1_swap = 0;
          }
          else
          {
            Map1_swap = 1;
          }
        }

        if(!NesHeader.v_rom_size) 
        {
          if ( Map1_Regs[ 0 ] & 0x10 )
          {
            bank_num <<= 2;
#if 0
            PPUBANK[ 4 ] = VRAMPAGE0( bank_num+0 );
            PPUBANK[ 5 ] = VRAMPAGE0( bank_num+1 );
            PPUBANK[ 6 ] = VRAMPAGE0( bank_num+2 );
            PPUBANK[ 7 ] = VRAMPAGE0( bank_num+3 );
#else
            PPUBANK[ 4 ] = CRAMPAGE( bank_num+0 );
            PPUBANK[ 5 ] = CRAMPAGE( bank_num+1 );
            PPUBANK[ 6 ] = CRAMPAGE( bank_num+2 );
            PPUBANK[ 7 ] = CRAMPAGE( bank_num+3 );
#endif
            InfoNES_SetupChr();
            break;
          }
        }

        // set 4K VROM bank at $1000  设置 $1000 处的 4K VROM 银行
        if(Map1_Regs[0] & 0x10)
        {
          // swap 4K  交换 4K
          bank_num <<= 2;
          PPUBANK[ 4 ] = VROMPAGE( (bank_num+0) % (NesHeader.v_rom_size << 3) );
          PPUBANK[ 5 ] = VROMPAGE( (bank_num+1) % (NesHeader.v_rom_size << 3) );
          PPUBANK[ 6 ] = VROMPAGE( (bank_num+2) % (NesHeader.v_rom_size << 3) );
          PPUBANK[ 7 ] = VROMPAGE( (bank_num+3) % (NesHeader.v_rom_size << 3) );
          InfoNES_SetupChr(); 
        }
      }
      break;

    case 3:
      {
        uint8_t bank_num = Map1_Regs[3];

        // set ROM bank  设置 ROM 银行
        if ( Map1_Regs[0] & 0x08 )
        {
          // 16K of ROM  16K 程序 ROM
          bank_num <<= 1;

          if ( Map1_Regs[0] & 0x04 )
          {
            // 16K of ROM at $8000  $8000 处的 16K ROM
            Map1_bank1 = bank_num;
            Map1_bank2 = bank_num+1;
            Map1_bank3 = Map1_HI1;
            Map1_bank4 = Map1_HI2;
          }
          else
          {
            // 16K of ROM at $C000  $C000 处的 16K ROM
            if(Map1_Size == Map1_SMALL)
            {
              Map1_bank1 = 0;
              Map1_bank2 = 1;
              Map1_bank3 = bank_num;
              Map1_bank4 = bank_num+1;
            }
          }
        }
        else
        {
          // 32K of ROM at $8000  $8000 处的 32K ROM
          bank_num <<= 1;

          Map1_bank1 = bank_num;
          Map1_bank2 = bank_num+1;
          if(Map1_Size == Map1_SMALL)
          {
            Map1_bank3 = bank_num+2;
            Map1_bank4 = bank_num+3;
          }
        }
        Map1_set_ROM_banks();
      }
      break;
  }
}
