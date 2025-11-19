#ifndef __CPU_H__
#define __CPU_H__

#include <stdint.h> 

// flags status
#define FlagCarry             0b00000001
#define FlagZero              0b00000010
#define FlagInterruptDisabled 0b00000100
#define FlagDecimalMode       0b00001000
#define FlagBreakCammand      0b00010000
#define FlagUnused            0b00100000
#define FlagOverflow          0b01000000
#define FlagNegative          0b10000000

typedef struct {
    uint8_t A;   // accumulator register
    uint8_t X;   // index register
    uint8_t Y;   // index register
    uint8_t SP;  // stack pointer
    uint16_t PC; // program pointer
    uint8_t P;   // processor status register
} reg_t;

typedef struct {
    reg_t reg; 

    uint8_t NMI; 
    uint8_t IRQ;
} CPU;

reg_t* get_reg();
CPU* get_cpu();
uint8_t get_NMI();
uint8_t get_IRQ();
void set_NMI(uint8_t);
void set_IRQ(uint8_t);
void cpu_reset();
void print_registers();

#endif