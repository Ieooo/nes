#include "cpu.h"
#include <stdio.h>

CPU cpu;

reg_t* get_reg() {
    return &cpu.reg;
}

CPU* get_cpu() {
    return &cpu;
}

uint8_t get_NMI() {
    return cpu.NMI;
}

uint8_t get_IRQ() {
    return cpu.IRQ;
}

void set_NMI(uint8_t v) {
    cpu.NMI = v;
}

void set_IRQ(uint8_t v) {
    cpu.IRQ = v;
}

void cpu_reset() {
    cpu.reg.P = 0x00 | FlagInterruptDisabled | FlagCarry | FlagZero;
    cpu.reg.PC = 0xFFFC;
    cpu.reg.SP = 0xFD;
}

void print_registers() {
    reg_t reg = cpu.reg;
    printf("PC: 0x%04X SP: 0x%02X P: 0x%02X A: 0x%02X X: 0x%02X Y: 0x%02X\n",
        reg.PC, reg.SP, reg.P, reg.A, reg.X, reg.Y);
}