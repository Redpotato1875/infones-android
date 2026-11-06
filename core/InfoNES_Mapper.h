/*===================================================================*/
/*                                                                   */
/*  InfoNES_Mapper.h : InfoNES Mapper Function  InfoNES_Mapper.h：InfoNES 映射器函数 */
/*                                                                   */
/*  2000/05/16  InfoNES Project ( based on NesterJ and nes_x )  InfoNES 项目（基于 NesterJ and nes_x） */
/*                                                                   */
/*===================================================================*/

#ifndef InfoNES_MAPPER_H_INCLUDED
#define InfoNES_MAPPER_H_INCLUDED

/*-------------------------------------------------------------------*/
/*  Include files  包含文件                                          */
/*-------------------------------------------------------------------*/

#include "InfoNES_Types.h"

/*-------------------------------------------------------------------*/
/*  Constants  常量                                                  */
/*-------------------------------------------------------------------*/

#define DRAM_SIZE    0xA000

/*-------------------------------------------------------------------*/
/*  Mapper resources  映射器资源                                     */
/*-------------------------------------------------------------------*/

/* Disk System RAM  磁盘系统 RAM（FDS） */
extern uint8_t DRAM[];

/*-------------------------------------------------------------------*/
/*  Macros  宏                                                       */
/*-------------------------------------------------------------------*/

/* The address of 8Kbytes unit of the ROM  ROM 的 8KB 单元地址 */
#define ROMPAGE(a)     &ROM[ (a) * 0x2000 ]
/* From behind the ROM, the address of 8kbytes unit  从 ROM 末尾倒数 8KB 单元的地址 */
#define ROMLASTPAGE(a) &ROM[ NesHeader.rom_size * 0x4000 - ( (a) + 1 ) * 0x2000 ]
/* The address of 1Kbytes unit of the VROM  VROM 的 1KB 单元地址 */
#define VROMPAGE(a)    &VROM[ (a) * 0x400 ]
/* The address of 1Kbytes unit of the CRAM  CRAM 的 1KB 单元地址 */
#define CRAMPAGE(a)   &PPURAM[ 0x0000 + ((a)&0x1F) * 0x400 ]
/* The address of 1Kbytes unit of the VRAM  VRAM 的 1KB 单元地址 */
#define VRAMPAGE(a)    &PPURAM[ 0x2000 + (a) * 0x400 ]
/* Translate the pointer to ChrBuf into the address of Pattern Table  将 ChrBuf 指针换算为图案表地址 */
#define PATTBL(a)      ( ( (a) - ChrBuf ) >> 2 )

/*-------------------------------------------------------------------*/
/*  Macros ( Mapper specific )  宏（映射器专用）                     */
/*-------------------------------------------------------------------*/

/* The address of 8Kbytes unit of the Map5 ROM  Map5 ROM 的 8KB 单元地址 */
#define Map5_ROMPAGE(a)     &Map5_Wram[ ( (a) & 0x07 ) * 0x2000 ]
/* The address of 1Kbytes unit of the Map6 Chr RAM  Map6 字符 RAM 的 1KB 单元地址 */
#define Map6_VROMPAGE(a)    &Map6_Chr_Ram[ (a) * 0x400 ]
/* The address of 1Kbytes unit of the Map19 Chr RAM  Map19 字符 RAM 的 1KB 单元地址 */
#define Map19_VROMPAGE(a)   &Map19_Chr_Ram[ (a) * 0x400 ]
/* The address of 1Kbytes unit of the Map85 Chr RAM  Map85 字符 RAM 的 1KB 单元地址 */
#define Map85_VROMPAGE(a)   &Map85_Chr_Ram[ (a) * 0x400 ]

/*-------------------------------------------------------------------*/
/*  Table of Mapper initialize function  映射器初始化函数表          */
/*-------------------------------------------------------------------*/

struct MapperTable_tag
{
  int mapper_no;
  void (*mapper_init)();
};

extern struct MapperTable_tag MapperTable[];

/*-------------------------------------------------------------------*/
/*  Function prototypes  函数原型                                    */
/*-------------------------------------------------------------------*/

void Map0_Init();
void Map0_Write( uint16_t address, uint8_t byte_data );
void Map0_Sram( uint16_t address, uint8_t byte_data );
void Map0_Apu( uint16_t address, uint8_t byte_data );
uint8_t Map0_ReadApu( uint16_t address );
void Map0_VSync();
void Map0_HSync();
void Map0_PPU( uint16_t address );
void Map0_RenderScreen( uint8_t op_mode );

void Map1_Init();
void Map1_Write( uint16_t address, uint8_t byte_data );
void Map1_set_ROM_banks();

void Map2_Init();
void Map2_Write( uint16_t address, uint8_t byte_data );

void Map3_Init();
void Map3_Write( uint16_t address, uint8_t byte_data );

void Map4_Init();
void Map4_Write( uint16_t address, uint8_t byte_data );
void Map4_HSync();
void Map4_Set_CPU_Banks();
void Map4_Set_PPU_Banks();

void Map5_Init();
void Map5_Write( uint16_t address, uint8_t byte_data );
void Map5_Apu( uint16_t address, uint8_t byte_data );
uint8_t Map5_ReadApu( uint16_t address );
void Map5_HSync();
void Map5_RenderScreen( uint8_t op_mode );
void Map5_Sync_Prg_Banks( void );

void Map6_Init();
void Map6_Write( uint16_t address, uint8_t byte_data );
void Map6_Apu( uint16_t address, uint8_t byte_data );
void Map6_HSync();

void Map7_Init();
void Map7_Write( uint16_t address, uint8_t byte_data );

void Map8_Init();
void Map8_Write( uint16_t address, uint8_t byte_data );

void Map9_Init();
void Map9_Write( uint16_t address, uint8_t byte_data );
void Map9_PPU( uint16_t address );

void Map10_Init();
void Map10_Write( uint16_t address, uint8_t byte_data );
void Map10_PPU( uint16_t address );

void Map11_Init();
void Map11_Write( uint16_t address, uint8_t byte_data );

void Map13_Init();
void Map13_Write( uint16_t address, uint8_t byte_data );

void Map15_Init();
void Map15_Write( uint16_t address, uint8_t byte_data );

void Map16_Init();
void Map16_Write( uint16_t address, uint8_t byte_data );
void Map16_HSync();

void Map17_Init();
void Map17_Apu( uint16_t address, uint8_t byte_data );
void Map17_HSync();

void Map18_Init();
void Map18_Write( uint16_t address, uint8_t byte_data );
void Map18_HSync();

void Map19_Init();
void Map19_Write( uint16_t address, uint8_t byte_data );
void Map19_Apu( uint16_t address, uint8_t byte_data );
uint8_t Map19_ReadApu( uint16_t address );
void Map19_HSync();

void Map21_Init();
void Map21_Write( uint16_t address, uint8_t byte_data );
void Map21_HSync();

void Map22_Init();
void Map22_Write( uint16_t address, uint8_t byte_data );

void Map23_Init();
void Map23_Write( uint16_t address, uint8_t byte_data );
void Map23_HSync();

void Map24_Init();
void Map24_Write( uint16_t address, uint8_t byte_data );
void Map24_HSync();

void Map25_Init();
void Map25_Write( uint16_t address, uint8_t byte_data );
void Map25_Sync_Vrom( int bank_select );
void Map25_HSync();

void Map26_Init();
void Map26_Write( uint16_t address, uint8_t byte_data );
void Map26_HSync();

void Map32_Init();
void Map32_Write( uint16_t address, uint8_t byte_data );

void Map33_Init();
void Map33_Write( uint16_t address, uint8_t byte_data );
void Map33_HSync();

void Map34_Init();
void Map34_Write( uint16_t address, uint8_t byte_data );
void Map34_Sram( uint16_t address, uint8_t byte_data );

void Map40_Init();
void Map40_Write( uint16_t address, uint8_t byte_data );
void Map40_HSync();

void Map41_Init();
void Map41_Write( uint16_t address, uint8_t byte_data );
void Map41_Sram( uint16_t address, uint8_t byte_data );

void Map42_Init();
void Map42_Write( uint16_t address, uint8_t byte_data );
void Map42_HSync();

void Map43_Init();
void Map43_Write( uint16_t address, uint8_t byte_data );
void Map43_Apu( uint16_t address, uint8_t byte_data );
uint8_t Map43_ReadApu( uint16_t address );
void Map43_HSync();

void Map44_Init();
void Map44_Write( uint16_t address, uint8_t byte_data );
void Map44_HSync();
void Map44_Set_CPU_Banks();
void Map44_Set_PPU_Banks();

void Map45_Init();
void Map45_Sram( uint16_t address, uint8_t byte_data );
void Map45_Write( uint16_t address, uint8_t byte_data );
void Map45_HSync();
void Map45_Set_CPU_Bank4( uint8_t byte_data );
void Map45_Set_CPU_Bank5( uint8_t byte_data );
void Map45_Set_CPU_Bank6( uint8_t byte_data );
void Map45_Set_CPU_Bank7( uint8_t byte_data );
void Map45_Set_PPU_Banks();

void Map46_Init();
void Map46_Sram( uint16_t address, uint8_t byte_data );
void Map46_Write( uint16_t address, uint8_t byte_data );
void Map46_Set_ROM_Banks();

void Map47_Init();
void Map47_Sram( uint16_t address, uint8_t byte_data );
void Map47_Write( uint16_t address, uint8_t byte_data );
void Map47_HSync();
void Map47_Set_CPU_Banks();
void Map47_Set_PPU_Banks();

void Map48_Init();
void Map48_Write( uint16_t address, uint8_t byte_data );
void Map48_HSync();

void Map49_Init();
void Map49_Sram( uint16_t address, uint8_t byte_data );
void Map49_Write( uint16_t address, uint8_t byte_data );
void Map49_HSync();
void Map49_Set_CPU_Banks();
void Map49_Set_PPU_Banks();

void Map50_Init();
void Map50_Apu( uint16_t address, uint8_t byte_data );
void Map50_HSync();

void Map51_Init();
void Map51_Sram( uint16_t address, uint8_t byte_data );
void Map51_Write( uint16_t address, uint8_t byte_data );
void Map51_Set_CPU_Banks();

void Map57_Init();
void Map57_Write( uint16_t address, uint8_t byte_data );

void Map58_Init();
void Map58_Write( uint16_t address, uint8_t byte_data );

void Map60_Init();
void Map60_Write( uint16_t address, uint8_t byte_data );

void Map61_Init();
void Map61_Write( uint16_t address, uint8_t byte_data );

void Map62_Init();
void Map62_Write( uint16_t address, uint8_t byte_data );

void Map64_Init();
void Map64_Write( uint16_t address, uint8_t byte_data );

void Map65_Init();
void Map65_Write( uint16_t address, uint8_t byte_data );
void Map65_HSync();

void Map66_Init();
void Map66_Write( uint16_t address, uint8_t byte_data );

void Map67_Init();
void Map67_Write( uint16_t address, uint8_t byte_data );
void Map67_HSync();

void Map68_Init();
void Map68_Write( uint16_t address, uint8_t byte_data );
void Map68_SyncMirror();

void Map69_Init();
void Map69_Write( uint16_t address, uint8_t byte_data );
void Map69_HSync();

void Map70_Init();
void Map70_Write( uint16_t address, uint8_t byte_data );

void Map71_Init();
void Map71_Write( uint16_t address, uint8_t byte_data );

void Map72_Init();
void Map72_Write( uint16_t address, uint8_t byte_data );

void Map73_Init();
void Map73_Write( uint16_t address, uint8_t byte_data );
void Map73_HSync();

void Map74_Init();
void Map74_Write( uint16_t address, uint8_t byte_data );
void Map74_HSync();
void Map74_Set_CPU_Banks();
void Map74_Set_PPU_Banks();

void Map75_Init();
void Map75_Write( uint16_t address, uint8_t byte_data );

void Map76_Init();
void Map76_Write( uint16_t address, uint8_t byte_data );

void Map77_Init();
void Map77_Write( uint16_t address, uint8_t byte_data );

void Map78_Init();
void Map78_Write( uint16_t address, uint8_t byte_data );

void Map79_Init();
void Map79_Apu( uint16_t address, uint8_t byte_data );

void Map80_Init();
void Map80_Sram( uint16_t address, uint8_t byte_data );

void Map82_Init();
void Map82_Sram( uint16_t address, uint8_t byte_data );

void Map83_Init();
void Map83_Write( uint16_t address, uint8_t byte_data );
void Map83_Apu( uint16_t address, uint8_t byte_data );
uint8_t Map83_ReadApu( uint16_t address );
void Map83_HSync();

void Map85_Init();
void Map85_Write( uint16_t address, uint8_t byte_data );
void Map85_HSync();

void Map86_Init();
void Map86_Sram( uint16_t address, uint8_t byte_data );

void Map87_Init();
void Map87_Sram( uint16_t address, uint8_t byte_data );

void Map88_Init();
void Map88_Write( uint16_t address, uint8_t byte_data );

void Map89_Init();
void Map89_Write( uint16_t address, uint8_t byte_data );

void Map90_Init();
void Map90_Write( uint16_t address, uint8_t byte_data );
void Map90_Apu( uint16_t address, uint8_t byte_data );
uint8_t Map90_ReadApu( uint16_t address );
void Map90_HSync();
void Map90_Sync_Mirror( void );
void Map90_Sync_Prg_Banks( void );
void Map90_Sync_Chr_Banks( void );

void Map91_Init();
void Map91_Sram( uint16_t address, uint8_t byte_data );

void Map92_Init();
void Map92_Write( uint16_t address, uint8_t byte_data );

void Map93_Init();
void Map93_Sram( uint16_t address, uint8_t byte_data );

void Map94_Init();
void Map94_Write( uint16_t address, uint8_t byte_data );

void Map95_Init();
void Map95_Write( uint16_t address, uint8_t byte_data );
void Map95_Set_CPU_Banks();
void Map95_Set_PPU_Banks();

void Map96_Init();
void Map96_Write( uint16_t address, uint8_t byte_data );
void Map96_PPU( uint16_t address );
void Map96_Set_Banks();

void Map97_Init();
void Map97_Write( uint16_t address, uint8_t byte_data );

void Map100_Init();
void Map100_Write( uint16_t address, uint8_t byte_data );
void Map100_HSync();
void Map100_Set_CPU_Banks();
void Map100_Set_PPU_Banks();

void Map101_Init();
void Map101_Write( uint16_t address, uint8_t byte_data );

void Map105_Init();
void Map105_Write( uint16_t address, uint8_t byte_data );
void Map105_HSync();

void Map107_Init();
void Map107_Write( uint16_t address, uint8_t byte_data );

void Map108_Init();
void Map108_Write( uint16_t address, uint8_t byte_data );

void Map109_Init();
void Map109_Apu( uint16_t address, uint8_t byte_data );
void Map109_Set_PPU_Banks();

void Map110_Init();
void Map110_Apu( uint16_t address, uint8_t byte_data );

void Map112_Init();
void Map112_Write( uint16_t address, uint8_t byte_data );
void Map112_HSync();
void Map112_Set_CPU_Banks();
void Map112_Set_PPU_Banks();

void Map113_Init();
void Map113_Apu( uint16_t address, uint8_t byte_data );
void Map113_Write( uint16_t address, uint8_t byte_data );

void Map114_Init();
void Map114_Sram( uint16_t address, uint8_t byte_data );
void Map114_Write( uint16_t address, uint8_t byte_data );
void Map114_HSync();
void Map114_Set_CPU_Banks();
void Map114_Set_PPU_Banks();

void Map115_Init();
void Map115_Sram( uint16_t address, uint8_t byte_data );
void Map115_Write( uint16_t address, uint8_t byte_data );
void Map115_HSync();
void Map115_Set_CPU_Banks();
void Map115_Set_PPU_Banks();

void Map116_Init();
void Map116_Write( uint16_t address, uint8_t byte_data );
void Map116_HSync();
void Map116_Set_CPU_Banks();
void Map116_Set_PPU_Banks();

void Map117_Init();
void Map117_Write( uint16_t address, uint8_t byte_data );
void Map117_HSync();

void Map118_Init();
void Map118_Write( uint16_t address, uint8_t byte_data );
void Map118_HSync();
void Map118_Set_CPU_Banks();
void Map118_Set_PPU_Banks();

void Map119_Init();
void Map119_Write( uint16_t address, uint8_t byte_data );
void Map119_HSync();
void Map119_Set_CPU_Banks();
void Map119_Set_PPU_Banks();

void Map122_Init();
void Map122_Sram( uint16_t address, uint8_t byte_data );

void Map133_Init();
void Map133_Apu( uint16_t address, uint8_t byte_data );

void Map134_Init();
void Map134_Apu( uint16_t address, uint8_t byte_data );

void Map135_Init();
void Map135_Apu( uint16_t address, uint8_t byte_data );
void Map135_Set_PPU_Banks();

void Map140_Init();
void Map140_Sram( uint16_t address, uint8_t byte_data );
void Map140_Apu( uint16_t address, uint8_t byte_data );

void Map151_Init();
void Map151_Write( uint16_t address, uint8_t byte_data );

void Map160_Init();
void Map160_Write( uint16_t address, uint8_t byte_data );
void Map160_HSync();

void Map180_Init();
void Map180_Write( uint16_t address, uint8_t byte_data );

void Map181_Init();
void Map181_Apu( uint16_t address, uint8_t byte_data );

void Map182_Init();
void Map182_Write( uint16_t address, uint8_t byte_data );
void Map182_HSync();

void Map183_Init();
void Map183_Write( uint16_t address, uint8_t byte_data );
void Map183_HSync();

void Map185_Init();
void Map185_Write( uint16_t address, uint8_t byte_data );

void Map187_Init();
void Map187_Write( uint16_t address, uint8_t byte_data );
void Map187_Apu( uint16_t address, uint8_t byte_data );
uint8_t Map187_ReadApu( uint16_t address );
void Map187_HSync();
void Map187_Set_CPU_Banks();
void Map187_Set_PPU_Banks();

void Map188_Init();
void Map188_Write( uint16_t address, uint8_t byte_data );

void Map189_Init();
void Map189_Apu( uint16_t address, uint8_t byte_data );
void Map189_Write( uint16_t address, uint8_t byte_data );
void Map189_HSync();

void Map191_Init();
void Map191_Apu( uint16_t address, uint8_t byte_data );
void Map191_Set_CPU_Banks();
void Map191_Set_PPU_Banks();

void Map193_Init();
void Map193_Sram( uint16_t address, uint8_t byte_data );

void Map194_Init();
void Map194_Write( uint16_t address, uint8_t byte_data );

void Map200_Init();
void Map200_Write( uint16_t address, uint8_t byte_data );

void Map201_Init();
void Map201_Write( uint16_t address, uint8_t byte_data );

void Map202_Init();
void Map202_Apu( uint16_t address, uint8_t byte_data );
void Map202_Write( uint16_t address, uint8_t byte_data );
void Map202_WriteSub( uint16_t address, uint8_t byte_data );

void Map222_Init();
void Map222_Write( uint16_t address, uint8_t byte_data );

void Map225_Init();
void Map225_Write( uint16_t address, uint8_t byte_data );

void Map226_Init();
void Map226_Write( uint16_t address, uint8_t byte_data );

void Map227_Init();
void Map227_Write( uint16_t address, uint8_t byte_data );

void Map228_Init();
void Map228_Write( uint16_t address, uint8_t byte_data );

void Map229_Init();
void Map229_Write( uint16_t address, uint8_t byte_data );

void Map230_Init();
void Map230_Write( uint16_t address, uint8_t byte_data );

void Map231_Init();
void Map231_Write( uint16_t address, uint8_t byte_data );

void Map232_Init();
void Map232_Write( uint16_t address, uint8_t byte_data );

void Map233_Init();
void Map233_Write( uint16_t address, uint8_t byte_data );

void Map234_Init();
void Map234_Write( uint16_t address, uint8_t byte_data );
void Map234_Set_Banks();

void Map235_Init();
void Map235_Write( uint16_t address, uint8_t byte_data );

void Map236_Init();
void Map236_Write( uint16_t address, uint8_t byte_data );

void Map240_Init();
void Map240_Apu( uint16_t address, uint8_t byte_data );

void Map241_Init();
void Map241_Write( uint16_t address, uint8_t byte_data );

void Map242_Init();
void Map242_Write( uint16_t address, uint8_t byte_data );

void Map243_Init();
void Map243_Apu( uint16_t address, uint8_t byte_data );

void Map244_Init();
void Map244_Write( uint16_t address, uint8_t byte_data );

void Map245_Init();
void Map245_Write( uint16_t address, uint8_t byte_data );
void Map245_HSync();
#if 0
void Map245_Set_CPU_Banks();
void Map245_Set_PPU_Banks();
#endif 

void Map246_Init();
void Map246_Sram( uint16_t address, uint8_t byte_data );

void Map248_Init();
void Map248_Write( uint16_t address, uint8_t byte_data );
void Map248_Apu( uint16_t address, uint8_t byte_data );
void Map248_Sram( uint16_t address, uint8_t byte_data );
void Map248_HSync();
void Map248_Set_CPU_Banks();
void Map248_Set_PPU_Banks();

void Map249_Init();
void Map249_Write( uint16_t address, uint8_t byte_data );
void Map249_Apu( uint16_t address, uint8_t byte_data );
void Map249_HSync();

void Map251_Init();
void Map251_Write( uint16_t address, uint8_t byte_data );
void Map251_Sram( uint16_t address, uint8_t byte_data );
void Map251_Set_Banks();

void Map252_Init();
void Map252_Write( uint16_t address, uint8_t byte_data );
void Map252_HSync();

void Map255_Init();
void Map255_Write( uint16_t address, uint8_t byte_data );
void Map255_Apu( uint16_t address, uint8_t byte_data );
uint8_t Map255_ReadApu( uint16_t address );

#endif /* !InfoNES_MAPPER_H_INCLUDED */
