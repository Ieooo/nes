#include "memory.h"

uint8_t memory[MEM_SIZE] = {0};

void mem_init() {
    uint8_t data_j[4] = {
        0x4C, 0x00, 0x00
    };
    set_bytes(0xFFFC, data_j, 4);

    uint8_t data_p[128] = {
        0xA9, 0x00,              // LDA #$00
        0x8D, 0x00, 0x02,        // STA $0200
        0xA2, 0x10,              // LDX #$10
        0xA0, 0x20,              // LDY #$20
        0xAD, 0x00, 0x02,        // LDA $0200
        0xF0, 0x06,              // BEQ test
        0xD0, 0x02,              // BNE skip
        0xA9, 0xFF,              // LDA #$FF
        0x8D, 0x00, 0x02,        // STA $0200
        0x4C, 0x08, 0x10,        // JMP done
        0x0A,                    // ASL A
        0x6A,                    // ROR A
        0x29, 0x0F,              // AND #$0F
        0x49, 0xFF,              // EOR #$FF
        0x09, 0xA0,              // ORA #$A0
        0xA2, 0x00,              // LDX #$00
        0x8E, 0x01, 0x02,        // STX $0201
        0xA0, 0xFF,              // LDY #$FF
        0x8C, 0x02, 0x02,        // STY $0202
        0xAE, 0x01, 0x02,        // LDX $0201
        0xAC, 0x02, 0x02,        // LDY $0202
        0x48,                    // PHA
        0x8A,                    // TXA
        0x48,                    // PHA
        0x68,                    // PLA
        0xAA,                    // TAX
        0x68,                    // PLA
        0x38,                    // SEC
        0xE9, 0x01,              // SBC #$01
        0x18,                    // CLC
        0x69, 0x01,              // ADC #$01
        0x90, 0x06,              // BCC noCarry
        0xB0, 0x02,              // BCS hasCarry
        0xA9, 0x00,              // LDA #$00
        0x8D, 0x03, 0x02,        // STA $0203
        0x4C, 0x0F, 0x10         // JMP finish
    };
    set_bytes(0, data_p, 128);
}

uint8_t get_byte(uint16_t addr) {
    return memory[addr];
}

uint16_t get_word(uint16_t addr) {
    return ((uint16_t)memory[addr+1] << 8) + (uint16_t)memory[addr];
}

void get_bytes(uint8_t* data, uint16_t addr, uint32_t len) {
    for (uint16_t i=0; i < len; i++) {
       data[(uint16_t)i] = get_byte(addr+(uint16_t)i);
    }
}

void set_byte(uint16_t addr, uint8_t value) {
    memory[addr] = value;
}

void set_word(uint16_t addr, uint16_t value) {
    memory[addr] = (uint8_t)value;
    memory[addr+1] = (uint8_t)(value>>8);
}

void set_bytes(uint16_t addr, uint8_t* data, uint32_t len) {
    for (uint32_t i=0; i < len && addr+i < MEM_SIZE; i++) {
        set_byte(addr+(uint16_t)i, data[(uint16_t)i]);
    }
}
