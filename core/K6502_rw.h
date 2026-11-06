/*===================================================================*/
/*                                                                   */
/*  K6502_RW.h : 6502 Reading/Writing Operation for NES  K6502_RW.h：NES 的 6502 读写操作 */
/*               This file is included in K6502.cpp  本文件由 K6502.cpp 包含 */
/*                                                                   */
/*  2000/5/23   InfoNES Project ( based on nes_x )  InfoNES 项目（基于 nes_x） */
/*                                                                   */
/*===================================================================*/

#ifndef K6502_RW_H_INCLUDED
#define K6502_RW_H_INCLUDED

/*-------------------------------------------------------------------*/
/*  Include files  包含文件                                          */
/*-------------------------------------------------------------------*/

#include "InfoNES.h"
#include "InfoNES_System.h"
#include "InfoNES_pAPU.h"

/*===================================================================*/
/*                                                                   */
/*            K6502_ReadZp() : Reading from the zero page  K6502_ReadZp()：从零页读取 */
/*                                                                   */
/*===================================================================*/
static inline uint8_t K6502_ReadZp( uint8_t bus_addr )
{
/*
 *  Reading from the zero page  从零页读取
 *
 *  Parameters  参数
 *    uint8_t bus_addr              (Read)  （只读入参）
 *      An address inside the zero page  零页内的地址
 *
 *  Return values  返回值
 *    Read Data  读取数据
 */

  return RAM[ bus_addr ];
}

/*===================================================================*/
/*                                                                   */
/*               K6502_Read() : Reading operation  K6502_Read()：读操作 */
/*                                                                   */
/*===================================================================*/
static inline uint8_t K6502_Read( uint16_t address )
{
/*
 *  Reading operation  读操作
 *
 *  Parameters  参数
 *    uint16_t address              (Read)  （只读入参）
 *      Address to read  读取地址
 *
 *  Return values  返回值
 *    Read data  读取数据
 *
 *  Remarks  备注
 *    0x0000 - 0x1fff  RAM ( 0x800 - 0x1fff is mirror of 0x0 - 0x7ff )  0x0000-0x1fff RAM（0x800-0x1fff 是 0x0-0x7ff 的镜像）
 *    0x2000 - 0x3fff  PPU  该地址段映射到 PPU
 *    0x4000 - 0x5fff  Sound  该地址段映射到声音寄存器
 *    0x6000 - 0x7fff  SRAM ( Battery Backed )  该地址段映射到电池备份 SRAM
 *    0x8000 - 0xffff  ROM  该地址段映射到程序 ROM
 *
 */
  uint8_t return_value;

  switch ( address & 0xe000 )
  {
    case 0x0000:  /* RAM  随机存取存储器 */
      return RAM[ address & 0x7ff ];

    case 0x2000:  /* PPU */
      if ( ( address & 0x7 ) == 0x7 )   /* PPU Memory  PPU 内存 */
      {
        uint16_t addr = PPU_Addr;

        // Increment PPU Address  递增 PPU 地址
        PPU_Addr += PPU_Increment;
        addr &= 0x3fff;

        // Set return value;  设置返回值
        return_value = PPU_R7;

        // Read PPU Memory  读取 PPU 内存
        PPU_R7 = PPUBANK[ addr >> 10 ][ addr & 0x3ff ];

        return return_value;
      }
      else
      if ( ( address & 0x7 ) == 0x4 )   /* SPR_RAM I/O Register  精灵 RAM I/O 寄存器 */
      {
        return SPRRAM[ PPU_R3++ ];
      }
      else                            
      if ( ( address & 0x7 ) == 0x2 )   /* PPU Status  PPU 状态 */
      {
        // Set return value  设置返回值
        return_value = PPU_R2;

        // Reset a V-Blank flag  清除场消隐标志
        PPU_R2 &= ~R2_IN_VBLANK;

        // Reset address latch  重置地址锁存
        PPU_Latch_Flag = 0;

        // Make a Nametable 0 in V-Blank  场消隐期间改用命名表 0
        if ( PPU_Scanline >= SCAN_VBLANK_START && !( PPU_R0 & R0_NMI_VB ) )
        {
          PPU_R0 &= ~R0_NAME_ADDR;
          PPU_NameTableBank = NAME_TABLE0;
        }
        return return_value;
      }
      break;

    case 0x4000:  /* Sound  声音 */
      if ( address == 0x4015 )
      {
        // APU control  APU 控制
        return_value = APU_Reg[ 0x4015 ];
	if ( ApuC1Atl > 0 ) return_value |= (1<<0);
	if ( ApuC2Atl > 0 ) return_value |= (1<<1);
	if (  !ApuC3Holdnote ) {
	  if ( ApuC3Atl > 0 ) return_value |= (1<<2);
	} else {
	  if ( ApuC3Llc > 0 ) return_value |= (1<<2);
	}
	if ( ApuC4Atl > 0 ) return_value |= (1<<3);

	// FrameIRQ  帧 IRQ
        APU_Reg[ 0x4015 ] &= ~0x40;
        return return_value;
      }
      else
      if ( address == 0x4016 )
      {
        // Set Joypad1 data  设置手柄 1 数据
        return_value = (uint8_t)( ( PAD1_Latch >> PAD1_Bit ) & 1 ) | 0x40;
        PAD1_Bit = ( PAD1_Bit == 23 ) ? 0 : ( PAD1_Bit + 1 );
        return return_value;
      }
      else
      if ( address == 0x4017 )
      {
        // Set Joypad2 data  设置手柄 2 数据
        return_value = (uint8_t)( ( PAD2_Latch >> PAD2_Bit ) & 1 ) | 0x40;
        PAD2_Bit = ( PAD2_Bit == 23 ) ? 0 : ( PAD2_Bit + 1 );
        return return_value;
      }
      else 
      {
        /* Return Mapper Register*/
        return MapperReadApu( address );
      }
      break;
      // The other sound registers are not readable.  其余声音寄存器不可读

    case 0x6000:  /* SRAM  存档 RAM */
      if ( ROM_SRAM )
      {
        return SRAM[ address & 0x1fff ];
      } else {    /* SRAM BANK  SRAM 银行 */
        return SRAMBANK[ address & 0x1fff ];
      }

    case 0x8000:  /* ROM BANK 0  ROM 银行 0 */
      return ROMBANK0[ address & 0x1fff ];

    case 0xa000:  /* ROM BANK 1  ROM 银行 1 */
      return ROMBANK1[ address & 0x1fff ];

    case 0xc000:  /* ROM BANK 2  ROM 银行 2 */
      return ROMBANK2[ address & 0x1fff ];

    case 0xe000:  /* ROM BANK 3  ROM 银行 3 */
      return ROMBANK3[ address & 0x1fff ];
  }

  return ( address >> 8 ); /* when a register is not readable the upper half
                            address is returned. */
}

/*===================================================================*/
/*                                                                   */
/*               K6502_Write() : Writing operation  K6502_Write()：写操作 */
/*                                                                   */
/*===================================================================*/
static inline void K6502_Write( uint16_t address, uint8_t byte_data )
{
/*
 *  Writing operation  写操作
 *
 *  Parameters  参数
 *    uint16_t address              (Read)  （只读入参）
 *      Address to write  写入地址
 *
 *    uint8_t byte_data             (Read)  （只读入参）
 *      Data to write  写入的数据
 *
 *  Remarks  备注
 *    0x0000 - 0x1fff  RAM ( 0x800 - 0x1fff is mirror of 0x0 - 0x7ff )  0x0000-0x1fff RAM（0x800-0x1fff 是 0x0-0x7ff 的镜像）
 *    0x2000 - 0x3fff  PPU  该地址段映射到 PPU
 *    0x4000 - 0x5fff  Sound  该地址段映射到声音寄存器
 *    0x6000 - 0x7fff  SRAM ( Battery Backed )  该地址段映射到电池备份 SRAM
 *    0x8000 - 0xffff  ROM  该地址段映射到程序 ROM
 *
 */

  switch ( address & 0xe000 )
  {
    case 0x0000:  /* RAM  随机存取存储器 */
      RAM[ address & 0x7ff ] = byte_data;
      break;

    case 0x2000:  /* PPU */
      switch ( address & 0x7 )
      {
        case 0:    /* 0x2000 */
          PPU_R0 = byte_data;
          PPU_Increment = ( PPU_R0 & R0_INC_ADDR ) ? 32 : 1;
          PPU_NameTableBank = NAME_TABLE0 + ( PPU_R0 & R0_NAME_ADDR );
          PPU_BG_Base = ( PPU_R0 & R0_BG_ADDR ) ? ChrBuf + 256 * 64 : ChrBuf;
          PPU_SP_Base = ( PPU_R0 & R0_SP_ADDR ) ? ChrBuf + 256 * 64 : ChrBuf;
          PPU_SP_Height = ( PPU_R0 & R0_SP_SIZE ) ? 16 : 8;

          // Account for Loopy's scrolling discoveries  依据 Loopy 的卷轴原理
		      PPU_Temp = ( PPU_Temp & 0xF3FF ) | ( ( ( (uint16_t)byte_data ) & 0x0003 ) << 10 );
          break;

        case 1:   /* 0x2001 */
          PPU_R1 = byte_data;
          break;

        case 2:   /* 0x2002 */
#if 0	  
          PPU_R2 = byte_data;     // 0x2002 is not writable  0x2002 不可写
#endif
          break;

        case 3:   /* 0x2003 */
          // Sprite RAM Address  精灵 RAM 地址
          PPU_R3 = byte_data;
          break;

        case 4:   /* 0x2004 */
          // Write data to Sprite RAM  向精灵 RAM 写数据
          SPRRAM[ PPU_R3++ ] = byte_data;
          break;

        case 5:   /* 0x2005 */
          // Set Scroll Register  设置卷轴寄存器
          if ( PPU_Latch_Flag )
          {
            // V-Scroll Register  垂直卷轴寄存器
            PPU_Scr_V_Next = ( byte_data > 239 ) ? 0 : byte_data;
            PPU_Scr_V_Byte_Next = PPU_Scr_V_Next >> 3;
            PPU_Scr_V_Bit_Next = PPU_Scr_V_Next & 7;

            // Added : more Loopy Stuff  补充：更多 Loopy 相关内容
			      PPU_Temp = ( PPU_Temp & 0xFC1F ) | ( ( ( (uint16_t)byte_data ) & 0xF8 ) << 2);
			      PPU_Temp = ( PPU_Temp & 0x8FFF ) | ( ( ( (uint16_t)byte_data ) & 0x07 ) << 12);
          }
          else
          {
            // H-Scroll Register  水平卷轴寄存器
            PPU_Scr_H_Next = byte_data;
            PPU_Scr_H_Byte_Next = PPU_Scr_H_Next >> 3;
            PPU_Scr_H_Bit_Next = PPU_Scr_H_Next & 7;

            // Added : more Loopy Stuff  补充：更多 Loopy 相关内容
			      PPU_Temp = ( PPU_Temp & 0xFFE0 ) | ( ( ( (uint16_t)byte_data ) & 0xF8 ) >> 3 );
          }
          PPU_Latch_Flag ^= 1;
          break;

        case 6:   /* 0x2006 */
          // Set PPU Address  设置 PPU 地址
          if ( PPU_Latch_Flag )
          {
            /* Low */
#if 0
            PPU_Addr = ( PPU_Addr & 0xff00 ) | ( (uint16_t)byte_data );
#else
            PPU_Temp = ( PPU_Temp & 0xFF00 ) | ( ( (uint16_t)byte_data ) & 0x00FF);
	    PPU_Addr = PPU_Temp;
#endif
            InfoNES_SetupScr();
          }
          else
          {
            /* High  高字节 */
#if 0
            PPU_Addr = ( PPU_Addr & 0x00ff ) | ( (uint16_t)( byte_data & 0x3f ) << 8 );
            InfoNES_SetupScr();
#else
            PPU_Temp = ( PPU_Temp & 0x00FF ) | ( ( ((uint16_t)byte_data) & 0x003F ) << 8 );
#endif            
          }
          PPU_Latch_Flag ^= 1;
          break;

        case 7:   /* 0x2007 */
          {
            uint16_t addr = PPU_Addr;
            
            // Increment PPU Address  递增 PPU 地址
            PPU_Addr += PPU_Increment;
            addr &= 0x3fff;

            // Write to PPU Memory  写入 PPU 内存
            if ( addr < 0x2000 && vram_write_enable )
            {
              // Pattern Data  图案数据
              ChrBufUpdate |= ( 1 << ( addr >> 10 ) );
              PPUBANK[ addr >> 10 ][ addr & 0x3ff ] = byte_data;
            }
            else
            if ( addr < 0x3f00 )  /* 0x2000 - 0x3eff */
            {
              // Name Table and mirror  命名表与镜像
              PPUBANK[   addr            >> 10 ][ addr & 0x3ff ] = byte_data;
              PPUBANK[ ( addr ^ 0x1000 ) >> 10 ][ addr & 0x3ff ] = byte_data;
            }
            else
            if ( !( addr & 0xf ) )  /* 0x3f00 or 0x3f10 */
            {
              // Palette mirror  调色板镜像
              PPURAM[ 0x3f10 ] = PPURAM[ 0x3f14 ] = PPURAM[ 0x3f18 ] = PPURAM[ 0x3f1c ] = 
              PPURAM[ 0x3f00 ] = PPURAM[ 0x3f04 ] = PPURAM[ 0x3f08 ] = PPURAM[ 0x3f0c ] = byte_data;
              PalTable[ 0x00 ] = PalTable[ 0x04 ] = PalTable[ 0x08 ] = PalTable[ 0x0c ] =
              PalTable[ 0x10 ] = PalTable[ 0x14 ] = PalTable[ 0x18 ] = PalTable[ 0x1c ] = NesPalette[ byte_data ] | 0x8000;
            }
            else
            if ( addr & 3 )
            {
              // Palette  调色板
              PPURAM[ addr ] = byte_data;
              PalTable[ addr & 0x1f ] = NesPalette[ byte_data ];
            }
          }
          break;
      }
      break;

    case 0x4000:  /* Sound  声音 */
      switch ( address & 0x1f )
      {
        case 0x00:
        case 0x01:
        case 0x02:
        case 0x03:          
        case 0x04:
        case 0x05:
        case 0x06:
        case 0x07:
        case 0x08:
        case 0x09:
        case 0x0a:
        case 0x0b:
        case 0x0c:
        case 0x0d:
        case 0x0e:
        case 0x0f:
        case 0x10:
        case 0x11:	  
        case 0x12:
        case 0x13:
          // Call Function corresponding to Sound Registers  调用与声音寄存器对应的函数
          if ( !APU_Mute )
            apu_sound_regs[ address & 0x1f ]( address, byte_data );
          break;

        case 0x14:  /* 0x4014 */
          // Sprite DMA  精灵 DMA
          switch ( byte_data >> 5 )
          {
            case 0x0:  /* RAM  随机存取存储器 */
              InfoNES_MemoryCopy( SPRRAM, &RAM[ ( (uint16_t)byte_data << 8 ) & 0x7ff ], SPRRAM_SIZE );
              break;

            case 0x3:  /* SRAM  存档 RAM */
              InfoNES_MemoryCopy( SPRRAM, &SRAM[ ( (uint16_t)byte_data << 8 ) & 0x1fff ], SPRRAM_SIZE );
              break;

            case 0x4:  /* ROM BANK 0  ROM 银行 0 */
              InfoNES_MemoryCopy( SPRRAM, &ROMBANK0[ ( (uint16_t)byte_data << 8 ) & 0x1fff ], SPRRAM_SIZE );
              break;

            case 0x5:  /* ROM BANK 1  ROM 银行 1 */
              InfoNES_MemoryCopy( SPRRAM, &ROMBANK1[ ( (uint16_t)byte_data << 8 ) & 0x1fff ], SPRRAM_SIZE );
              break;

            case 0x6:  /* ROM BANK 2  ROM 银行 2 */
              InfoNES_MemoryCopy( SPRRAM, &ROMBANK2[ ( (uint16_t)byte_data << 8 ) & 0x1fff ], SPRRAM_SIZE );
              break;

            case 0x7:  /* ROM BANK 3  ROM 银行 3 */
              InfoNES_MemoryCopy( SPRRAM, &ROMBANK3[ ( (uint16_t)byte_data << 8 ) & 0x1fff ], SPRRAM_SIZE );
              break;
          }
          break;

        case 0x15:  /* 0x4015 */
          InfoNES_pAPUWriteControl( address, byte_data );
#if 0
          /* Unknown  未知 */
          if ( byte_data & 0x10 ) 
          {
	    byte_data &= ~0x80;
	  }
#endif
          break;

        case 0x16:  /* 0x4016 */
          // Reset joypad  重置手柄
          if ( !( APU_Reg[ 0x16 ] & 1 ) && ( byte_data & 1 ) )
          {
            PAD1_Bit = 0;
            PAD2_Bit = 0;
          }
          break;

        case 0x17:  /* 0x4017 */
          // Frame IRQ  帧 IRQ
          FrameStep = 0;
          if ( !( byte_data & 0xc0 ) )
          {
            FrameIRQ_Enable = 1;
          } else {
            FrameIRQ_Enable = 0;
          }
          break;
      }

      if ( address <= 0x4017 )
      {
        /* Write to APU Register  写入 APU 寄存器 */
        APU_Reg[ address & 0x1f ] = byte_data;
      }
      else
      {
        /* Write to APU  写入 APU */
        MapperApu( address, byte_data );
      }
      break;

    case 0x6000:  /* SRAM  存档 RAM */
      SRAM[ address & 0x1fff ] = byte_data;

      /* Write to SRAM, when no SRAM  没有 SRAM 时写入 SRAM 区 */
      if ( !ROM_SRAM )
      {
        MapperSram( address, byte_data );
      }
      break;

    case 0x8000:  /* ROM BANK 0  ROM 银行 0 */
    case 0xa000:  /* ROM BANK 1  ROM 银行 1 */
    case 0xc000:  /* ROM BANK 2  ROM 银行 2 */
    case 0xe000:  /* ROM BANK 3  ROM 银行 3 */
      // Write to Mapper  写入映射器
      MapperWrite( address, byte_data );
      break;
  }
}

// Reading/Writing operation (uint16_t version)  读写操作（uint16_t 版本）
static inline uint16_t K6502_ReadW( uint16_t address ){ return K6502_Read( address ) | (uint16_t)K6502_Read( address + 1 ) << 8; };
static inline void K6502_WriteW( uint16_t address, uint16_t word_data ){ K6502_Write( address, word_data & 0xff ); K6502_Write( address + 1, word_data >> 8 ); };
static inline uint16_t K6502_ReadZpW( uint8_t bus_addr ){ return K6502_ReadZp( bus_addr ) | ( K6502_ReadZp( bus_addr + 1 ) << 8 ); };

// 6502's indirect absolute jmp(opcode: 6C) has a bug (added at 01/08/15 )  6502 的间接绝对跳转（操作码 6C）存在硬件缺陷（01/08/15 加入）
static inline uint16_t K6502_ReadW2( uint16_t address )
{ 
  if ( 0x00ff == ( address & 0x00ff ) )
  {
    return K6502_Read( address ) | (uint16_t)K6502_Read( address - 0x00ff ) << 8;
  } else {
    return K6502_Read( address ) | (uint16_t)K6502_Read( address + 1 ) << 8;
  }
}

#endif /* !K6502_RW_H_INCLUDED */
