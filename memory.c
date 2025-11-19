#include "memory.h"
#include <stdio.h>
#include <stdbool.h>

memory_t* memory;

memory_t* mem_init(mapper_t* mapper, PPU *ppu) {
    memory = malloc(sizeof(memory_t));
    if (!memory) {
        return NULL;
    }
    memory->mem = malloc(sizeof(uint8_t)*MEM_SIZE);
    if (!memory->mem) {
        free(memory);
        return NULL;
    }
    if (mapper) {
        memory->mapper = mapper;
    }
    if (ppu) {
        memory->ppu = ppu;
    }
    return memory;
}

void mem_exit() {
    if(!memory) {
        mapper_exit(memory->mapper);
        free(memory);
    }
}

uint8_t get_byte(uint16_t addr) {
    if (memory->mapper && addr >= 0x4020 && addr <= 0xFFFF) {
        // mapper
        return memory->mapper->get_byte(memory->mapper, addr);
    } else if(memory->ppu && addr >= 0x2000 && addr <= 0x3FFF) {
        // ppu register
        return ppu_get_register(memory->ppu, (addr % 8) + 0x2000);
    } else if(memory->apu && addr >= 0x4000 && addr <= 0x4017) {
        // apu register 
        return apu_get_register(memory->apu, addr);
    } else {
        if (memory->mapper && addr >= 0x0800 && addr <= 0x1FFF) {
            // mapping  [0x0000 - 0x07FF]
            return memory->mem[addr % 0x0800];
        } else {
            return memory->mem[addr];
        }
    }
}

void set_byte(uint16_t addr, uint8_t value) {
    if (memory->mapper && (addr >= 0x4020 && addr <= 0xFFFF)) {
        // mapper
        memory->mapper->set_byte(memory->mapper, addr, value);
    } else if(memory->ppu && addr >= 0x2000 && addr <= 0x3FFF) {
        // ppu register
        ppu_write_register(memory->ppu, 0x2000 + (addr % 8), value);
    } else if(addr == PPU_DMC) {
        // ppu dmc
        // printf("================= DMC write[%04x]\n", (uint16_t)value << 8);
        memcpy(memory->ppu->oam, memory->mem + ((uint16_t)value << 8), 0xFF); 
    } else if(memory->apu && addr >= 0x4000 && addr <= 0x4017) {
        // apu register 
        apu_write_register(memory->apu, addr, value);
    } else {
        if (memory->mapper && addr >= 0x0800 && addr <= 0x1FFF) {
            // mapping  [0x0000 - 0x07FF]
            memory->mem[addr % 0x0800] = value;
        } else {
            memory->mem[addr] = value;
        }
    }
}

uint16_t get_word(uint16_t addr) {
    uint8_t low = get_byte(addr);
    uint8_t high = get_byte(addr+1);
    return ((uint16_t)high << 8) + (uint16_t)low;
}

void get_bytes(uint8_t* data, uint16_t addr, uint32_t len) {
    for (uint16_t i=0; i < len; i++) {
       data[(uint16_t)i] = get_byte(addr+(uint16_t)i);
    }
}

void set_bytes(uint16_t addr, uint8_t* data, uint32_t len) {
    for (uint32_t i=0; i < len && addr+i < MEM_SIZE; i++) {
        set_byte(addr+(uint16_t)i, data[(uint16_t)i]);
    }
}
