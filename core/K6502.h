/*===================================================================*/
/*                                                                   */
/*  K6502.h : Header file for K6502  K6502.h：K6502 头文件           */
/*                                                                   */
/*  2000/05/29  InfoNES Project ( based on nes_x )  InfoNES 项目（基于 nes_x） */
/*                                                                   */
/*===================================================================*/

#ifndef K6502_H_INCLUDED
#define K6502_H_INCLUDED

#include "InfoNES_Types.h"

/* 6502 Flags  6502 标志 */
#define FLAG_C 0x01
#define FLAG_Z 0x02
#define FLAG_I 0x04
#define FLAG_D 0x08
#define FLAG_B 0x10
#define FLAG_R 0x20
#define FLAG_V 0x40
#define FLAG_N 0x80

/* Stack Address  堆栈地址 */
#define BASE_STACK   0x100

/* Interrupt Vectors  中断向量 */
#define VECTOR_NMI   0xfffa
#define VECTOR_RESET 0xfffc
#define VECTOR_IRQ   0xfffe

// NMI Request  NMI 请求
#define NMI_REQ  NMI_State = 0;

// IRQ Request  IRQ 请求
#define IRQ_REQ  IRQ_State = 0;

// Emulator Operation  模拟器操作
void K6502_Init();
void K6502_Reset();
void K6502_Set_Int_Wiring( uint8_t nmi_wiring, uint8_t irq_wiring );
void K6502_Step( register uint16_t cycle_count );

// I/O Operation (User definition)  输入输出操作（用户定义）
static inline uint8_t K6502_Read( uint16_t address);
static inline uint16_t K6502_ReadW( uint16_t address );
static inline uint16_t K6502_ReadW2( uint16_t address );
static inline uint8_t K6502_ReadZp( uint8_t bus_addr );
static inline uint16_t K6502_ReadZpW( uint8_t bus_addr );
static inline uint8_t K6502_ReadAbsX();
static inline uint8_t K6502_ReadAbsY();
static inline uint8_t K6502_ReadIY();

static inline void K6502_Write( uint16_t address, uint8_t byte_data );
static inline void K6502_WriteW( uint16_t address, uint16_t word_data );

// The state of the IRQ pin  IRQ 引脚状态
extern uint8_t IRQ_State;

// The state of the NMI pin  NMI 引脚状态
extern uint8_t NMI_State;

// The number of the clocks that it passed  已经过的时钟数
extern uint16_t g_wPassedClocks;

#endif /* !K6502_H_INCLUDED */
