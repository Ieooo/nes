#ifndef __INSTRUCTION_H__
#define __INSTRUCTION_H__

#include <stdint.h>
#include <string.h>
#include "cpu.h"
#include "memory.h"

// addressing mode
typedef enum {
    NO,
    IMPLIED,
    REGISTER,
    RELATIVE,
    IMMEDIATE,
    ZERO_PAGE,
    ZERO_PAGE_X,
    ZERO_PAGE_Y,
    ABSOLUTE,
    ABSOLUTE_X,
    ABSOLUTE_Y,
    INDIRECT,
    INDIRECT_X,
    INDIRECT_Y
} addr_mode;

typedef struct {
    char* name;
    uint8_t length;        // instruction length(byte)
    uint8_t mode;          // addressing mode
    uint8_t cycle;         // how many cycle cost to execute
    // result is error code if result < 0,
    // result is additational clock cycle if result >= 0
    int (*exec_func)(exector_arg_t);
} instruction_t;

typedef struct {
    instruction_t instr;
    uint8_t other[5];
} exector_arg_t;


instruction_t decode_instruction(uint8_t);

int LDA(exector_arg_t);
int LDX(exector_arg_t);
int LDY(exector_arg_t);
int STA(exector_arg_t);
int STX(exector_arg_t);
int STY(exector_arg_t);

int ADC(exector_arg_t);
int SBC(exector_arg_t);
int INC(exector_arg_t);
int INX(exector_arg_t);
int INY(exector_arg_t);
int DEC(exector_arg_t);
int DEX(exector_arg_t);
int DEY(exector_arg_t);
int CMP(exector_arg_t);
int CPX(exector_arg_t);
int CPY(exector_arg_t);

int AND(exector_arg_t);
int ORA(exector_arg_t);
int EOR(exector_arg_t);
int ASL(exector_arg_t);
int LSR(exector_arg_t);
int ROL(exector_arg_t);
int ROR(exector_arg_t);
int BIT(exector_arg_t);

int JMP(exector_arg_t);
int JSR(exector_arg_t);
int RTS(exector_arg_t);
int BCC(exector_arg_t);
int BCS(exector_arg_t);
int BEQ(exector_arg_t);
int BMI(exector_arg_t);
int BNE(exector_arg_t);
int BPL(exector_arg_t);
int BVC(exector_arg_t);
int BVS(exector_arg_t);

int PHA(exector_arg_t);
int PHP(exector_arg_t);
int PLA(exector_arg_t);
int PLP(exector_arg_t);

int CLC(exector_arg_t);
int SEC(exector_arg_t);
int CLI(exector_arg_t);
int SEI(exector_arg_t);
int CLV(exector_arg_t);
int CLD(exector_arg_t);
int SED(exector_arg_t);
int NOP(exector_arg_t);
int BRK(exector_arg_t);

int TXS(exector_arg_t);
int TSX(exector_arg_t);
int TXA(exector_arg_t);
int TXA(exector_arg_t);
int TYA(exector_arg_t);
int TAX(exector_arg_t);
int TAY(exector_arg_t);

int RTI(exector_arg_t);

int NDF(exector_arg_t);

void print_asm();

#endif