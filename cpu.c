#include "cpu.h"

register_t reg;

register_t* get_reg() {
    return &reg;
}

void cpu_reset() {
    reg.A = 0;
    reg.X = 0;
    reg.Y = 0;
    reg.P = 0;
    reg.PC = 0xFFFC;
    reg.SP = 0xFD;
}

void print_registers() {
    printf("PC: 0x%04X SP: 0x%02X P: 0x%02X A: 0x%02X X: 0x%02X Y: 0x%02X\n",
        reg.PC, reg.SP, reg.P, reg.A, reg.X, reg.Y);
}