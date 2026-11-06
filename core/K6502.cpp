/*===================================================================*/
/*                                                                   */
/*  K6502.cpp : 6502 Emulator  K6502.cpp：6502 模拟器                */
/*                                                                   */
/*  2000/5/10   InfoNES Project ( based on nes_x )  InfoNES 项目（基于 nes_x） */
/*                                                                   */
/*===================================================================*/

/*-------------------------------------------------------------------*/
/*  Include files  包含文件                                          */
/*-------------------------------------------------------------------*/

#include "K6502.h"
#include "InfoNES_System.h"

/*-------------------------------------------------------------------*/
/*  Operation Macros  操作宏                                         */
/*-------------------------------------------------------------------*/

// Clock Op.  时钟操作
#define CLK(a)   g_wPassedClocks += (a);

// Addressing Op.  寻址操作
// Address  地址
// (Indirect,X)  变址间接 X
#define AA_IX    K6502_ReadZpW( K6502_Read( PC++ ) + X )
// (Indirect),Y  间接变址 Y
#define AA_IY    K6502_ReadZpW( K6502_Read( PC++ ) ) + Y
// Zero Page  零页
#define AA_ZP    K6502_Read( PC++ )
// Zero Page,X  零页变址 X
#define AA_ZPX   (uint8_t)( K6502_Read( PC++ ) + X )
// Zero Page,Y  零页变址 Y
#define AA_ZPY   (uint8_t)( K6502_Read( PC++ ) + Y )
// Absolute  绝对
#define AA_ABS   ( K6502_Read( PC++ ) | (uint16_t)K6502_Read( PC++ ) << 8 )
// Absolute2 ( PC-- )  绝对地址 2（PC--）
#define AA_ABS2  ( K6502_Read( PC++ ) | (uint16_t)K6502_Read( PC ) << 8 )
// Absolute,X  绝对变址 X
#define AA_ABSX  AA_ABS + X
// Absolute,Y  绝对变址 Y
#define AA_ABSY  AA_ABS + Y

// Data  数据
// (Indirect,X)  变址间接 X
#define A_IX    K6502_Read( AA_IX )
// (Indirect),Y  间接变址 Y
#define A_IY    K6502_ReadIY()
// Zero Page  零页
#define A_ZP    K6502_ReadZp( AA_ZP )
// Zero Page,X  零页变址 X
#define A_ZPX   K6502_ReadZp( AA_ZPX )
// Zero Page,Y  零页变址 Y
#define A_ZPY   K6502_ReadZp( AA_ZPY )
// Absolute  绝对
#define A_ABS   K6502_Read( AA_ABS )
// Absolute,X  绝对变址 X
#define A_ABSX  K6502_ReadAbsX()
// Absolute,Y  绝对变址 Y
#define A_ABSY  K6502_ReadAbsY()
// Immediate  立即
#define A_IMM   K6502_Read( PC++ )

// Flag Op.  标志操作
#define SETF(a)  F |= (a)
#define RSTF(a)  F &= ~(a)
#define TEST(a)  RSTF( FLAG_N | FLAG_Z ); SETF( g_byTestTable[ a ] )

// Load & Store Op.  加载与存储操作
#define STA(a)    K6502_Write( (a), A );
#define STX(a)    K6502_Write( (a), X );
#define STY(a)    K6502_Write( (a), Y );
#define LDA(a)    A = (a); TEST( A );
#define LDX(a)    X = (a); TEST( X );
#define LDY(a)    Y = (a); TEST( Y );

// Stack Op.  堆栈操作
#define PUSH(a)   K6502_Write( BASE_STACK + SP--, (a) )
#define PUSHW(a)  PUSH( (a) >> 8 ); PUSH( (a) & 0xff )
#define POP(a)    a = K6502_Read( BASE_STACK + ++SP )
#define POPW(a)   POP(a); a |= ( K6502_Read( BASE_STACK + ++SP ) << 8 )

// Logical Op.  逻辑操作
#define ORA(a)  A |= (a); TEST( A )
#define AND(a)  A &= (a); TEST( A )
#define EOR(a)  A ^= (a); TEST( A )
#define BIT(a)  byte_tmp = (a); RSTF( FLAG_N | FLAG_V | FLAG_Z ); SETF( ( byte_tmp & ( FLAG_N | FLAG_V ) ) | ( ( byte_tmp & A ) ? 0 : FLAG_Z ) );
#define CMP(a)  word_tmp = (uint16_t)A - (a); RSTF( FLAG_N | FLAG_Z | FLAG_C ); SETF( g_byTestTable[ word_tmp & 0xff ] | ( word_tmp < 0x100 ? FLAG_C : 0 ) );
#define CPX(a)  word_tmp = (uint16_t)X - (a); RSTF( FLAG_N | FLAG_Z | FLAG_C ); SETF( g_byTestTable[ word_tmp & 0xff ] | ( word_tmp < 0x100 ? FLAG_C : 0 ) );
#define CPY(a)  word_tmp = (uint16_t)Y - (a); RSTF( FLAG_N | FLAG_Z | FLAG_C ); SETF( g_byTestTable[ word_tmp & 0xff ] | ( word_tmp < 0x100 ? FLAG_C : 0 ) );
  
// Math Op. (A D flag isn't being supported.)  算术运算（不支持十进制 D 标志）
#define ADC(a)  byte_tmp = (a); \
                word_tmp = A + byte_tmp + ( F & FLAG_C ); \
                byte_tmp2 = (uint8_t)word_tmp; \
                RSTF( FLAG_N | FLAG_V | FLAG_Z | FLAG_C ); \
                SETF( g_byTestTable[ byte_tmp2 ] | ( ( ~( A ^ byte_tmp ) & ( A ^ byte_tmp2 ) & 0x80 ) ? FLAG_V : 0 ) | ( word_tmp > 0xff ) ); \
                A = byte_tmp2;

#define SBC(a)  byte_tmp = (a); \
                word_tmp = A - byte_tmp - ( ~F & FLAG_C ); \
                byte_tmp2 = (uint8_t)word_tmp; \
                RSTF( FLAG_N | FLAG_V | FLAG_Z | FLAG_C ); \
                SETF( g_byTestTable[ byte_tmp2 ] | ( ( ( A ^ byte_tmp ) & ( A ^ byte_tmp2 ) & 0x80 ) ? FLAG_V : 0 ) | ( word_tmp < 0x100 ) ); \
                A = byte_tmp2;

#define DEC(a)  addr_tmp = a; byte_tmp = K6502_Read( addr_tmp ); --byte_tmp; K6502_Write( addr_tmp, byte_tmp ); TEST( byte_tmp )
#define INC(a)  addr_tmp = a; byte_tmp = K6502_Read( addr_tmp ); ++byte_tmp; K6502_Write( addr_tmp, byte_tmp ); TEST( byte_tmp )

// Shift Op.  移位操作
#define ASLA    RSTF( FLAG_N | FLAG_Z | FLAG_C ); SETF( g_ASLTable[ A ].flag_value ); A = g_ASLTable[ A ].table_value 
#define ASL(a)  RSTF( FLAG_N | FLAG_Z | FLAG_C ); addr_tmp = a; byte_tmp = K6502_Read( addr_tmp ); SETF( g_ASLTable[ byte_tmp ].flag_value ); K6502_Write( addr_tmp, g_ASLTable[ byte_tmp ].table_value )
#define LSRA    RSTF( FLAG_N | FLAG_Z | FLAG_C ); SETF( g_LSRTable[ A ].flag_value ); A = g_LSRTable[ A ].table_value 
#define LSR(a)  RSTF( FLAG_N | FLAG_Z | FLAG_C ); addr_tmp = a; byte_tmp = K6502_Read( addr_tmp ); SETF( g_LSRTable[ byte_tmp ].flag_value ); K6502_Write( addr_tmp, g_LSRTable[ byte_tmp ].table_value ) 
#define ROLA    byte_tmp = F & FLAG_C; RSTF( FLAG_N | FLAG_Z | FLAG_C ); SETF( g_ROLTable[ byte_tmp ][ A ].flag_value ); A = g_ROLTable[ byte_tmp ][ A ].table_value 
#define ROL(a)  byte_tmp2 = F & FLAG_C; RSTF( FLAG_N | FLAG_Z | FLAG_C ); addr_tmp = a; byte_tmp = K6502_Read( addr_tmp ); SETF( g_ROLTable[ byte_tmp2 ][ byte_tmp ].flag_value ); K6502_Write( addr_tmp, g_ROLTable[ byte_tmp2 ][ byte_tmp ].table_value )
#define RORA    byte_tmp = F & FLAG_C; RSTF( FLAG_N | FLAG_Z | FLAG_C ); SETF( g_RORTable[ byte_tmp ][ A ].flag_value ); A = g_RORTable[ byte_tmp ][ A ].table_value 
#define ROR(a)  byte_tmp2 = F & FLAG_C; RSTF( FLAG_N | FLAG_Z | FLAG_C ); addr_tmp = a; byte_tmp = K6502_Read( addr_tmp ); SETF( g_RORTable[ byte_tmp2 ][ byte_tmp ].flag_value ); K6502_Write( addr_tmp, g_RORTable[ byte_tmp2 ][ byte_tmp ].table_value )

// Jump Op.  跳转操作
#define JSR     addr_tmp = AA_ABS2; PUSHW( PC ); PC = addr_tmp; 
#define BRA(a)  if ( a ) { addr_tmp = PC; PC += (char)K6502_Read( PC ); CLK( 3 + ( ( addr_tmp & 0x0100 ) != ( PC & 0x0100 ) ) ); ++PC; } else { ++PC; CLK( 2 ); }
#define JMP(a)  PC = a;

/*-------------------------------------------------------------------*/
/*  Global valiables  全局变量（原文拼写如此）                       */
/*-------------------------------------------------------------------*/

// 6502 Register  6502 寄存器
uint16_t PC;
uint8_t SP;
uint8_t F;
uint8_t A;
uint8_t X;
uint8_t Y;

// The state of the IRQ pin  IRQ 引脚状态
uint8_t IRQ_State;

// Wiring of the IRQ pin  IRQ 引脚接线方式
uint8_t IRQ_Wiring;

// The state of the NMI pin  NMI 引脚状态
uint8_t NMI_State;

// Wiring of the NMI pin  NMI 引脚接线方式
uint8_t NMI_Wiring;

// The number of the clocks that it passed  已经过的时钟数
uint16_t g_wPassedClocks;

// A table for the test  测试（BIT）查找表
uint8_t g_byTestTable[ 256 ];

// Value and Flag Data  数值与标志数据
struct value_table_tag
{
  uint8_t table_value;
  uint8_t flag_value;
};

// A table for ASL  ASL 查找表
struct value_table_tag g_ASLTable[ 256 ];

// A table for LSR  LSR 查找表
struct value_table_tag g_LSRTable[ 256 ];

// A table for ROL  ROL 查找表
struct value_table_tag g_ROLTable[ 2 ][ 256 ];

// A table for ROR  ROR 查找表
struct value_table_tag g_RORTable[ 2 ][ 256 ];

/*===================================================================*/
/*                                                                   */
/*                K6502_Init() : Initialize K6502  K6502_Init()：初始化 K6502 */
/*                                                                   */
/*===================================================================*/
void K6502_Init()
{
/*
 *  Initialize K6502  初始化 K6502
 *
 *  You must call this function only once at first.  该函数只能首次调用一次
 */

  uint8_t idx;
  uint8_t idx2;

  // The establishment of the IRQ pin  IRQ 引脚的置位
  NMI_Wiring = NMI_State = 1;
  IRQ_Wiring = IRQ_State = 1;

  // Make a table for the test  生成测试查找表
  idx = 0;
  do
  {
    if ( idx == 0 )
      g_byTestTable[ 0 ] = FLAG_Z;
    else
    if ( idx > 127 )
      g_byTestTable[ idx ] = FLAG_N;
    else
      g_byTestTable[ idx ] = 0;

    ++idx;
  } while ( idx != 0 );

  // Make a table ASL  生成 ASL 查找表
  idx = 0;
  do
  {
    g_ASLTable[ idx ].table_value = idx << 1;
    g_ASLTable[ idx ].flag_value = 0;

    if ( idx > 127 )
      g_ASLTable[ idx ].flag_value = FLAG_C;

    if ( g_ASLTable[ idx ].table_value == 0 )
      g_ASLTable[ idx ].flag_value |= FLAG_Z;
    else
    if ( g_ASLTable[ idx ].table_value & 0x80 )
      g_ASLTable[ idx ].flag_value |= FLAG_N;

    ++idx;
  } while ( idx != 0 );

  // Make a table LSR  生成 LSR 查找表
  idx = 0;
  do
  {
    g_LSRTable[ idx ].table_value = idx >> 1;
    g_LSRTable[ idx ].flag_value = 0;

    if ( idx & 1 )
      g_LSRTable[ idx ].flag_value = FLAG_C;

    if ( g_LSRTable[ idx ].table_value == 0 )
      g_LSRTable[ idx ].flag_value |= FLAG_Z;

    ++idx;
  } while ( idx != 0 );

  // Make a table ROL  生成 ROL 查找表
  for ( idx2 = 0; idx2 < 2; ++idx2 )
  {
    idx = 0;
    do
    {
      g_ROLTable[ idx2 ][ idx ].table_value = ( idx << 1 ) | idx2;
      g_ROLTable[ idx2 ][ idx ].flag_value = 0;

      if ( idx > 127 )
        g_ROLTable[ idx2 ][ idx ].flag_value = FLAG_C;

      if ( g_ROLTable[ idx2 ][ idx ].table_value == 0 )
        g_ROLTable[ idx2 ][ idx ].flag_value |= FLAG_Z;
      else
      if ( g_ROLTable[ idx2 ][ idx ].table_value & 0x80 )
        g_ROLTable[ idx2 ][ idx ].flag_value |= FLAG_N;

      ++idx;
    } while ( idx != 0 );
  }

  // Make a table ROR  生成 ROR 查找表
  for ( idx2 = 0; idx2 < 2; ++idx2 )
  {
    idx = 0;
    do
    {
      g_RORTable[ idx2 ][ idx ].table_value = ( idx >> 1 ) | ( idx2 << 7 );
      g_RORTable[ idx2 ][ idx ].flag_value = 0;

      if ( idx & 1 )
        g_RORTable[ idx2 ][ idx ].flag_value = FLAG_C;

      if ( g_RORTable[ idx2 ][ idx ].table_value == 0 )
        g_RORTable[ idx2 ][ idx ].flag_value |= FLAG_Z;
      else
      if ( g_RORTable[ idx2 ][ idx ].table_value & 0x80 )
        g_RORTable[ idx2 ][ idx ].flag_value |= FLAG_N;

      ++idx;
    } while ( idx != 0 );
  }
}

/*===================================================================*/
/*                                                                   */
/*                K6502_Reset() : Reset a CPU  K6502_Reset()：复位 CPU */
/*                                                                   */
/*===================================================================*/
void K6502_Reset()
{
/*
 *  Reset a CPU  复位 CPU
 *
 */

  // Reset Registers  复位寄存器
  PC = K6502_ReadW( VECTOR_RESET );
  SP = 0xFF;
  A = X = Y = 0;
  F = FLAG_Z | FLAG_R | FLAG_I;

  // Set up the state of the Interrupt pin.  设置中断引脚状态
  NMI_State = NMI_Wiring;
  IRQ_State = IRQ_Wiring;

  // Reset Passed Clocks  重置已经过的时钟
  g_wPassedClocks = 0;
}

/*===================================================================*/
/*                                                                   */
/*    K6502_Set_Int_Wiring() : Set up wiring of the interrupt pin  K6502_Set_Int_Wiring()：设置中断引脚接线方式 */
/*                                                                   */
/*===================================================================*/
void K6502_Set_Int_Wiring( uint8_t nmi_wiring, uint8_t irq_wiring )
{
/*
 * Set up wiring of the interrupt pin  设置中断引脚接线方式
 *
 */

  NMI_Wiring = nmi_wiring;
  IRQ_Wiring = irq_wiring;
}

/*===================================================================*/
/*                                                                   */
/*  K6502_Step() :  K6502_Step()：执行一条指令                       */
/*          Only the specified number of the clocks execute Op.  仅按指定时钟数执行操作 */
/*                                                                   */
/*===================================================================*/
void K6502_Step( uint16_t cycle_count )
{
/*
 *  Only the specified number of the clocks execute Op.  仅按指定时钟数执行操作
 *
 *  Parameters  参数
 *    uint16_t cycle_count              (Read)  （只读入参）
 *      The number of the clocks  时钟数
 */

  uint8_t code;

  uint16_t addr_tmp;
  uint8_t byte_tmp;
  uint8_t byte_tmp2;
  uint16_t word_tmp;

  // Dispose of it if there is an interrupt requirement  若有中断请求则处理它
  if ( NMI_State != NMI_Wiring )
  {
    // NMI Interrupt  NMI 中断
    NMI_State = NMI_Wiring;
    CLK( 7 );

    PUSHW( PC );
    PUSH( F & ~FLAG_B );

    RSTF( FLAG_D );
    SETF( FLAG_I );

    PC = K6502_ReadW( VECTOR_NMI );
  }
  else
  if ( IRQ_State != IRQ_Wiring )
  {
    // IRQ Interrupt  IRQ 中断
    // Execute IRQ if an I flag isn't being set  I 标志未置位时执行 IRQ
    if ( !( F & FLAG_I ) )
    {
      IRQ_State = IRQ_Wiring;
      CLK( 7 );

      PUSHW( PC );
      PUSH( F & ~FLAG_B );

      RSTF( FLAG_D );
      SETF( FLAG_I );
    
      PC = K6502_ReadW( VECTOR_IRQ );
    }
  }

  // It has a loop until a constant clock passes  循环执行直到经过固定时钟
  while ( g_wPassedClocks < cycle_count )
  {
    // Read an instruction  读取指令
    code = K6502_Read( PC++ );

    // Execute an instruction.  执行一条指令
    switch ( code )
    {
      case 0x00:  // BRK  软件中断
        ++PC; PUSHW( PC ); SETF( FLAG_B ); PUSH( F ); SETF( FLAG_I ); RSTF( FLAG_D ); PC = K6502_ReadW( VECTOR_IRQ ); CLK( 7 );
        break;

      case 0x01:  // ORA (Zpg,X)  按位或：变址间接 X
        ORA( A_IX ); CLK( 6 );
        break;

      case 0x05:  // ORA Zpg  按位或：零页寻址
        ORA( A_ZP ); CLK( 3 );
        break;

      case 0x06:  // ASL Zpg  左移：零页寻址
        ASL( AA_ZP ); CLK( 5 );
        break;

      case 0x08:  // PHP  标志压栈
        SETF( FLAG_B ); PUSH( F ); CLK( 3 );
        break;

      case 0x09:  // ORA #Oper  按位或：立即数寻址
        ORA( A_IMM ); CLK( 2 );
        break;

      case 0x0A:  // ASL A  左移：累加器 A
        ASLA; CLK( 2 );
        break;

      case 0x0D:  // ORA Abs  按位或：绝对寻址
        ORA( A_ABS ); CLK( 4 );
        break;

      case 0x0e:  // ASL Abs  左移：绝对寻址
        ASL( AA_ABS ); CLK( 6 );
        break;

      case 0x10: // BPL Oper
        BRA( !( F & FLAG_N ) );
        break;

      case 0x11: // ORA (Zpg),Y  按位或：间接变址 Y
        ORA( A_IY ); CLK( 5 );
        break;

      case 0x15: // ORA Zpg,X  按位或：零页变址 X
        ORA( A_ZPX ); CLK( 4 );
        break;

      case 0x16: // ASL Zpg,X  左移：零页变址 X
        ASL( AA_ZPX ); CLK( 6 );
        break;

      case 0x18: // CLC  清除进位
        RSTF( FLAG_C ); CLK( 2 );
        break;

      case 0x19: // ORA Abs,Y  按位或：绝对变址 Y
        ORA( A_ABSY ); CLK( 4 );
        break;

      case 0x1D: // ORA Abs,X  按位或：绝对变址 X
        ORA( A_ABSX ); CLK( 4 );
        break;

      case 0x1E: // ASL Abs,X  左移：绝对变址 X
        ASL( AA_ABSX ); CLK( 7 );
        break;

      case 0x20: // JSR Abs  子程序调用：绝对寻址
        JSR; CLK( 6 );
        break;

      case 0x21: // AND (Zpg,X)  按位与：变址间接 X
        AND( A_IX ); CLK( 6 );
        break;

      case 0x24: // BIT Zpg  位检测：零页寻址
        BIT( A_ZP ); CLK( 3 );
        break;

      case 0x25: // AND Zpg  按位与：零页寻址
        AND( A_ZP ); CLK( 3 );
        break;

      case 0x26: // ROL Zpg  循环左移：零页寻址
        ROL( AA_ZP ); CLK( 5 );
        break;

      case 0x28: // PLP  出栈到标志
        POP( F ); SETF( FLAG_R ); CLK( 4 );
        break;

      case 0x29: // AND #Oper  按位与：立即数寻址
        AND( A_IMM ); CLK( 2 );
        break;

      case 0x2A: // ROL A  循环左移：累加器 A
        ROLA; CLK( 2 );
        break;

      case 0x2C: // BIT Abs  位检测：绝对寻址
        BIT( A_ABS ); CLK( 4 );
        break;

      case 0x2D: // AND Abs  按位与：绝对寻址
        AND( A_ABS ); CLK( 4 );
        break;

      case 0x2E: // ROL Abs  循环左移：绝对寻址
        ROL( AA_ABS ); CLK( 6 );
        break;

      case 0x30: // BMI Oper 
        BRA( F & FLAG_N );
        break;

      case 0x31: // AND (Zpg),Y  按位与：间接变址 Y
        AND( A_IY ); CLK( 5 );
        break;

      case 0x35: // AND Zpg,X  按位与：零页变址 X
        AND( A_ZPX ); CLK( 4 );
        break;

      case 0x36: // ROL Zpg,X  循环左移：零页变址 X
        ROL( AA_ZPX ); CLK( 6 );
        break;

      case 0x38: // SEC  置进位
        SETF( FLAG_C ); CLK( 2 );
        break;

      case 0x39: // AND Abs,Y  按位与：绝对变址 Y
        AND( A_ABSY ); CLK( 4 );
        break;

      case 0x3D: // AND Abs,X  按位与：绝对变址 X
        AND( A_ABSX ); CLK( 4 );
        break;

      case 0x3E: // ROL Abs,X  循环左移：绝对变址 X
        ROL( AA_ABSX ); CLK( 7 );
        break;

      case 0x40: // RTI  中断返回
        POP( F ); SETF( FLAG_R ); POPW( PC ); CLK( 6 );
        break;

      case 0x41: // EOR (Zpg,X)  异或：变址间接 X
        EOR( A_IX ); CLK( 6 );
        break;

      case 0x45: // EOR Zpg  异或：零页寻址
        EOR( A_ZP ); CLK( 3 );
        break;

      case 0x46: // LSR Zpg  右移：零页寻址
        LSR( AA_ZP ); CLK( 5 );
        break;

      case 0x48: // PHA  A 压栈
        PUSH( A ); CLK( 3 );
        break;

      case 0x49: // EOR #Oper  异或：立即数寻址
        EOR( A_IMM ); CLK( 2 );
        break;

      case 0x4A: // LSR A  右移：累加器 A
        LSRA; CLK( 2 );
        break;

      case 0x4C: // JMP Abs  跳转：绝对寻址
        JMP( AA_ABS ); CLK( 3 );
        break;

      case 0x4D: // EOR Abs  异或：绝对寻址
        EOR( A_ABS ); CLK( 4 );
        break;

      case 0x4E: // LSR Abs  右移：绝对寻址
        LSR( AA_ABS ); CLK( 6 );
        break;

      case 0x50: // BVC  无溢出则跳转
        BRA( !( F & FLAG_V ) );
        break;

      case 0x51: // EOR (Zpg),Y  异或：间接变址 Y
        EOR( A_IY ); CLK( 5 );
        break;

      case 0x55: // EOR Zpg,X  异或：零页变址 X
        EOR( A_ZPX ); CLK( 4 );
        break;

      case 0x56: // LSR Zpg,X  右移：零页变址 X
        LSR( AA_ZPX ); CLK( 6 );
        break;

      case 0x58: // CLI  允许中断
        byte_tmp = F;
        RSTF( FLAG_I ); CLK( 2 );
        if ( ( byte_tmp & FLAG_I ) && IRQ_State != IRQ_Wiring )  
        {
          IRQ_State = IRQ_Wiring;          
          CLK( 7 );

          PUSHW( PC );
          PUSH( F & ~FLAG_B );

          RSTF( FLAG_D );
          SETF( FLAG_I );
    
          PC = K6502_ReadW( VECTOR_IRQ );
        }
        break;

      case 0x59: // EOR Abs,Y  异或：绝对变址 Y
        EOR( A_ABSY ); CLK( 4 );
        break;

      case 0x5D: // EOR Abs,X  异或：绝对变址 X
        EOR( A_ABSX ); CLK( 4 );
        break;

      case 0x5E: // LSR Abs,X  右移：绝对变址 X
        LSR( AA_ABSX ); CLK( 7 );
        break;

      case 0x60: // RTS  子程序返回
        POPW( PC ); ++PC; CLK( 6 );
        break;

      case 0x61: // ADC (Zpg,X)  加法：变址间接 X
        ADC( A_IX ); CLK( 6 );
        break;

      case 0x65: // ADC Zpg  加法：零页寻址
        ADC( A_ZP ); CLK( 3 );
        break;

      case 0x66: // ROR Zpg  循环右移：零页寻址
        ROR( AA_ZP ); CLK( 5 );
        break;

      case 0x68: // PLA  出栈到 A
        POP( A ); TEST( A ); CLK( 4 );
        break;

      case 0x69: // ADC #Oper  加法：立即数寻址
        ADC( A_IMM ); CLK( 2 );
        break;

      case 0x6A: // ROR A  循环右移：累加器 A
        RORA; CLK( 2 );
        break;

      case 0x6C: // JMP (Abs)  跳转：间接绝对寻址
        JMP( K6502_ReadW2( AA_ABS ) ); CLK( 5 );
        break;

      case 0x6D: // ADC Abs  加法：绝对寻址
        ADC( A_ABS ); CLK( 4 );
        break;

      case 0x6E: // ROR Abs  循环右移：绝对寻址
        ROR( AA_ABS ); CLK( 6 );
        break;

      case 0x70: // BVS  有溢出则跳转
        BRA( F & FLAG_V );
        break;

      case 0x71: // ADC (Zpg),Y  加法：间接变址 Y
        ADC( A_IY ); CLK( 5 );
        break;

      case 0x75: // ADC Zpg,X  加法：零页变址 X
        ADC( A_ZPX ); CLK( 4 );
        break;

      case 0x76: // ROR Zpg,X  循环右移：零页变址 X
        ROR( AA_ZPX ); CLK( 6 );
        break;

      case 0x78: // SEI  禁止中断
        SETF( FLAG_I ); CLK( 2 );
        break;

      case 0x79: // ADC Abs,Y  加法：绝对变址 Y
        ADC( A_ABSY ); CLK( 4 );
        break;

      case 0x7D: // ADC Abs,X  加法：绝对变址 X
        ADC( A_ABSX ); CLK( 4 );
        break;

      case 0x7E: // ROR Abs,X  循环右移：绝对变址 X
        ROR( AA_ABSX ); CLK( 7 );
        break;

      case 0x81: // STA (Zpg,X)  存 A：变址间接 X
        STA( AA_IX ); CLK( 6 );
        break;
      
      case 0x84: // STY Zpg  存 Y：零页寻址
        STY( AA_ZP ); CLK( 3 );
        break;

      case 0x85: // STA Zpg  存 A：零页寻址
        STA( AA_ZP ); CLK( 3 );
        break;

      case 0x86: // STX Zpg  存 X：零页寻址
        STX( AA_ZP ); CLK( 3 );
        break;

      case 0x88: // DEY  Y 寄存器自减
        --Y; TEST( Y ); CLK( 2 );
        break;

      case 0x8A: // TXA  X 传 A
        A = X; TEST( A ); CLK( 2 );
        break;

      case 0x8C: // STY Abs  存 Y：绝对寻址
        STY( AA_ABS ); CLK( 4 );
        break;

      case 0x8D: // STA Abs  存 A：绝对寻址
        STA( AA_ABS ); CLK( 4 );
        break;

      case 0x8E: // STX Abs  存 X：绝对寻址
        STX( AA_ABS ); CLK( 4 );
        break;

      case 0x90: // BCC  无进位则跳转
        BRA( !( F & FLAG_C ) );
        break;

      case 0x91: // STA (Zpg),Y  存 A：间接变址 Y
        STA( AA_IY ); CLK( 6 );
        break;

      case 0x94: // STY Zpg,X  存 Y：零页变址 X
        STY( AA_ZPX ); CLK( 4 );
        break;

      case 0x95: // STA Zpg,X  存 A：零页变址 X
        STA( AA_ZPX ); CLK( 4 );
        break;

      case 0x96: // STX Zpg,Y  存 X：零页变址 Y
        STX( AA_ZPY ); CLK( 4 );
        break;

      case 0x98: // TYA  Y 传 A
        A = Y; TEST( A ); CLK( 2 );
        break;

      case 0x99: // STA Abs,Y  存 A：绝对变址 Y
        STA( AA_ABSY ); CLK( 5 );
        break;

      case 0x9A: // TXS  X 传栈指针
        SP = X; CLK( 2 );
        break;

      case 0x9D: // STA Abs,X  存 A：绝对变址 X
        STA( AA_ABSX ); CLK( 5 );
        break;

      case 0xA0: // LDY #Oper  取数到 Y：立即数寻址
        LDY( A_IMM ); CLK( 2 );
        break;

      case 0xA1: // LDA (Zpg,X)  取数到 A：变址间接 X
        LDA( A_IX ); CLK( 6 );
        break;

      case 0xA2: // LDX #Oper  取数到 X：立即数寻址
        LDX( A_IMM ); CLK( 2 );
        break;

      case 0xA4: // LDY Zpg  取数到 Y：零页寻址
        LDY( A_ZP ); CLK( 3 );
        break;

      case 0xA5: // LDA Zpg  取数到 A：零页寻址
        LDA( A_ZP ); CLK( 3 );
        break;

      case 0xA6: // LDX Zpg  取数到 X：零页寻址
        LDX( A_ZP ); CLK( 3 );
        break;

      case 0xA8: // TAY  A 传 Y
        Y = A; TEST( A ); CLK( 2 );
        break;

      case 0xA9: // LDA #Oper  取数到 A：立即数寻址
        LDA( A_IMM ); CLK( 2 );
        break;

      case 0xAA: // TAX  A 传 X
        X = A; TEST( A ); CLK( 2 );
        break;

      case 0xAC: // LDY Abs  取数到 Y：绝对寻址
        LDY( A_ABS ); CLK( 4 );
        break;

      case 0xAD: // LDA Abs  取数到 A：绝对寻址
        LDA( A_ABS ); CLK( 4 );
        break;

      case 0xAE: // LDX Abs  取数到 X：绝对寻址
        LDX( A_ABS ); CLK( 4 );
        break;

      case 0xB0: // BCS  有进位则跳转
        BRA( F & FLAG_C );
        break;

      case 0xB1: // LDA (Zpg),Y  取数到 A：间接变址 Y
        LDA( A_IY ); CLK( 5 );
        break;

      case 0xB4: // LDY Zpg,X  取数到 Y：零页变址 X
        LDY( A_ZPX ); CLK( 4 );
        break;

      case 0xB5: // LDA Zpg,X  取数到 A：零页变址 X
        LDA( A_ZPX ); CLK( 4 );
        break;

      case 0xB6: // LDX Zpg,Y  取数到 X：零页变址 Y
        LDX( A_ZPY ); CLK( 4 );
        break;

      case 0xB8: // CLV  清除溢出
        RSTF( FLAG_V ); CLK( 2 );
        break;

      case 0xB9: // LDA Abs,Y  取数到 A：绝对变址 Y
        LDA( A_ABSY ); CLK( 4 );
        break;

      case 0xBA: // TSX  栈指针传 X
        X = SP; TEST( X ); CLK( 2 );
        break;

      case 0xBC: // LDY Abs,X  取数到 Y：绝对变址 X
        LDY( A_ABSX ); CLK( 4 );
        break;

      case 0xBD: // LDA Abs,X  取数到 A：绝对变址 X
        LDA( A_ABSX ); CLK( 4 );
        break;

      case 0xBE: // LDX Abs,Y  取数到 X：绝对变址 Y
        LDX( A_ABSY ); CLK( 4 );
        break;

      case 0xC0: // CPY #Oper  Y 寄存器比较：立即数寻址
        CPY( A_IMM ); CLK( 2 );
        break;

      case 0xC1: // CMP (Zpg,X)  比较：变址间接 X
        CMP( A_IX ); CLK( 6 );
        break;

      case 0xC4: // CPY Zpg  Y 寄存器比较：零页寻址
        CPY( A_ZP ); CLK( 3 );
        break;

      case 0xC5: // CMP Zpg  比较：零页寻址
        CMP( A_ZP ); CLK( 3 );
        break;

      case 0xC6: // DEC Zpg  自减：零页寻址
        DEC( AA_ZP ); CLK( 5 );
        break;

      case 0xC8: // INY  Y 寄存器自增
        ++Y; TEST( Y ); CLK( 2 );
        break;

      case 0xC9: // CMP #Oper  比较：立即数寻址
        CMP( A_IMM ); CLK( 2 );
        break;

      case 0xCA: // DEX  X 寄存器自减
        --X; TEST( X ); CLK( 2 );
        break;

      case 0xCC: // CPY Abs  Y 寄存器比较：绝对寻址
        CPY( A_ABS ); CLK( 4 );
        break;

      case 0xCD: // CMP Abs  比较：绝对寻址
        CMP( A_ABS ); CLK( 4 );
        break;

      case 0xCE: // DEC Abs  自减：绝对寻址
        DEC( AA_ABS ); CLK( 6 );
        break;

      case 0xD0: // BNE  不等则跳转
        BRA( !( F & FLAG_Z ) );
        break;

      case 0xD1: // CMP (Zpg),Y  比较：间接变址 Y
        CMP( A_IY ); CLK( 5 );
        break;

      case 0xD5: // CMP Zpg,X  比较：零页变址 X
        CMP( A_ZPX ); CLK( 4 );
        break;

      case 0xD6: // DEC Zpg,X  自减：零页变址 X
        DEC( AA_ZPX ); CLK( 6 );
        break;

      case 0xD8: // CLD  清除十进制
        RSTF( FLAG_D ); CLK( 2 );
        break;

      case 0xD9: // CMP Abs,Y  比较：绝对变址 Y
        CMP( A_ABSY ); CLK( 4 );
        break;

      case 0xDD: // CMP Abs,X  比较：绝对变址 X
        CMP( A_ABSX ); CLK( 4 );
        break;

      case 0xDE: // DEC Abs,X  自减：绝对变址 X
        DEC( AA_ABSX ); CLK( 7 );
        break;

      case 0xE0: // CPX #Oper  X 寄存器比较：立即数寻址
        CPX( A_IMM ); CLK( 2 );
        break;

      case 0xE1: // SBC (Zpg,X)  减法：变址间接 X
        SBC( A_IX ); CLK( 6 );
        break;

      case 0xE4: // CPX Zpg  X 寄存器比较：零页寻址
        CPX( A_ZP ); CLK( 3 );
        break;

      case 0xE5: // SBC Zpg  减法：零页寻址
        SBC( A_ZP ); CLK( 3 );
        break;

      case 0xE6: // INC Zpg  自增：零页寻址
        INC( AA_ZP ); CLK( 5 );
        break;

      case 0xE8: // INX  X 寄存器自增
        ++X; TEST( X ); CLK( 2 );
        break;

      case 0xE9: // SBC #Oper  减法：立即数寻址
        SBC( A_IMM ); CLK( 2 );
        break;

      case 0xEA: // NOP  空操作
        CLK( 2 );
        break;

      case 0xEC: // CPX Abs  X 寄存器比较：绝对寻址
        CPX( A_ABS ); CLK( 4 );
        break;

      case 0xED: // SBC Abs  减法：绝对寻址
        SBC( A_ABS ); CLK( 4 );
        break;

      case 0xEE: // INC Abs  自增：绝对寻址
        INC( AA_ABS ); CLK( 6 );
        break;

      case 0xF0: // BEQ  相等则跳转
        BRA( F & FLAG_Z );
        break;

      case 0xF1: // SBC (Zpg),Y  减法：间接变址 Y
        SBC( A_IY ); CLK( 5 );
        break;

      case 0xF5: // SBC Zpg,X  减法：零页变址 X
        SBC( A_ZPX ); CLK( 4 );
        break;

      case 0xF6: // INC Zpg,X  自增：零页变址 X
        INC( AA_ZPX ); CLK( 6 );
        break;

      case 0xF8: // SED  置十进制
        SETF( FLAG_D ); CLK( 2 );
        break;

      case 0xF9: // SBC Abs,Y  减法：绝对变址 Y
        SBC( A_ABSY ); CLK( 4 );
        break;

      case 0xFD: // SBC Abs,X  减法：绝对变址 X
        SBC( A_ABSX ); CLK( 4 );
        break;

      case 0xFE: // INC Abs,X  自增：绝对变址 X
        INC( AA_ABSX ); CLK( 7 );
        break;

      /*-----------------------------------------------------------*/
      /*  Unlisted Instructions ( thanks to virtualnes )  未列出的指令（感谢 virtualnes） */
      /*-----------------------------------------------------------*/

			case	0x1A: // NOP (Unofficial)  NOP（非官方指令）
			case	0x3A: // NOP (Unofficial)  NOP（非官方指令）
			case	0x5A: // NOP (Unofficial)  NOP（非官方指令）
			case	0x7A: // NOP (Unofficial)  NOP（非官方指令）
			case	0xDA: // NOP (Unofficial)  NOP（非官方指令）
			case	0xFA: // NOP (Unofficial)  NOP（非官方指令）
				CLK( 2 );
				break;

			case	0x80: // DOP (CYCLES 2)  双字节空操作（周期 2）
			case	0x82: // DOP (CYCLES 2)  双字节空操作（周期 2）
			case	0x89: // DOP (CYCLES 2)  双字节空操作（周期 2）
			case	0xC2: // DOP (CYCLES 2)  双字节空操作（周期 2）
			case	0xE2: // DOP (CYCLES 2)  双字节空操作（周期 2）
				PC++;
				CLK( 2 );
				break;

			case	0x04: // DOP (CYCLES 3)  双字节空操作（周期 3）
			case	0x44: // DOP (CYCLES 3)  双字节空操作（周期 3）
			case	0x64: // DOP (CYCLES 3)  双字节空操作（周期 3）
				PC++;
				CLK( 3 );
				break;

			case	0x14: // DOP (CYCLES 4)  双字节空操作（周期 4）
			case	0x34: // DOP (CYCLES 4)  双字节空操作（周期 4）
			case	0x54: // DOP (CYCLES 4)  双字节空操作（周期 4）
			case	0x74: // DOP (CYCLES 4)  双字节空操作（周期 4）
			case	0xD4: // DOP (CYCLES 4)  双字节空操作（周期 4）
			case	0xF4: // DOP (CYCLES 4)  双字节空操作（周期 4）
        PC++; 
        CLK( 4 );
        break;

			case	0x0C: // TOP
			case	0x1C: // TOP
			case	0x3C: // TOP
			case	0x5C: // TOP
			case	0x7C: // TOP
			case	0xDC: // TOP
			case	0xFC: // TOP
				PC+=2;
				CLK( 4 );
				break;

      default:   // Unknown Instruction  未知指令
        CLK( 2 );
#if 0
        InfoNES_MessageBox( "0x%02x is unknown instruction.\n", code ) ;
#endif
        break;
        
    }  /* end of switch ( code )  switch 结束（代码） */

  }  /* end of while ...  while 结束 */

  // Correct the number of the clocks  校正时钟数
  g_wPassedClocks -= cycle_count;
}

// Addressing Op.  寻址操作
// Data  数据
// Absolute,X  绝对变址 X
static inline uint8_t K6502_ReadAbsX(){ uint16_t addr_tmp, addr_tmp2; addr_tmp = AA_ABS; addr_tmp2 = addr_tmp + X; CLK( ( addr_tmp & 0x0100 ) != ( addr_tmp2 & 0x0100 ) ); return K6502_Read( addr_tmp2 ); };
// Absolute,Y  绝对变址 Y
static inline uint8_t K6502_ReadAbsY(){ uint16_t addr_tmp, addr_tmp2; addr_tmp = AA_ABS; addr_tmp2 = addr_tmp + Y; CLK( ( addr_tmp & 0x0100 ) != ( addr_tmp2 & 0x0100 ) ); return K6502_Read( addr_tmp2 ); };
// (Indirect),Y  间接变址 Y
static inline uint8_t K6502_ReadIY(){ uint16_t addr_tmp, addr_tmp2; addr_tmp = K6502_ReadZpW( K6502_Read( PC++ ) ); addr_tmp2 = addr_tmp + Y; CLK( ( addr_tmp & 0x0100 ) != ( addr_tmp2 & 0x0100 ) ); return K6502_Read( addr_tmp2 ); };

/*===================================================================*/
/*                                                                   */
/*                  6502 Reading/Writing Operation  6502 读写操作    */
/*                                                                   */
/*===================================================================*/
#include "K6502_rw.h"
