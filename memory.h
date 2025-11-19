#ifndef __MEMORY_H__
#define __MEMORY_H__

#include <stdint.h>
#include "mapper.h"
#include "ppu.h"
#include "apu.h"

#define MEM_SIZE 64*1024

#define STACK_BASE 0x01FF
#define STACK_TOP  0x0100

#define INTERRUPT_VECTOR_BRK 0xFFFE  // BRK/IRQ
#define INTERRUPT_VECTOR_NMI 0xFFFA
#define RESET_VECTOR         0xFFFC


typedef struct memory {
    uint8_t  *mem;
    mapper_t *mapper;
    PPU      *ppu;
    APU      *apu;
} memory_t;

memory_t* mem_init(mapper_t*, PPU*);
void mem_exit();
uint8_t get_byte(uint16_t addr);
uint16_t get_word(uint16_t addr);
void get_bytes(uint8_t* data, uint16_t addr, uint32_t len);
void set_byte(uint16_t addr, uint8_t value);
void set_bytes(uint16_t addr, uint8_t* data, uint32_t len);

#endif