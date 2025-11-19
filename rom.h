#ifndef __ROM_H__
#define __ROM_H__

#include <stdint.h>

typedef struct __attribute__((packed)) {
    char magic[4];          // magic: NES\x1A
    uint8_t PRG_rom_size;  // PRG rom size(16KB)
    uint8_t CHR_rom_size;  // CHR rom size(8KB)
    uint8_t flag6;
    uint8_t flag7;
    uint8_t PRG_ram_size;
    uint8_t tv_type;
    uint8_t flag10;
    uint8_t reserved[5];

} rom_header;

typedef struct {
    rom_header header;
    uint8_t* trainer;
    uint8_t* PRG_data;
    uint8_t* CHR_data;
} Rom;

Rom* load_nes_rom(char*);


#endif