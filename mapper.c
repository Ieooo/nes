#include "mapper.h"
#include "rom.h"
#include <stdio.h>

uint8_t mapper0_get_byte(mapper_t* mapper, uint16_t addr);
uint8_t mapper0_set_byte(mapper_t *mapper, uint16_t addr, uint8_t value);

mapper_t* mapper_init(Rom *rom) {
    mapper_t* mapper = malloc(sizeof(mapper_t));
    if (!mapper) {
        return NULL;
    }
    uint8_t mapper_type = (rom->header.flag7 & 0xF0) + ((rom->header.flag6 & 0xF0) >> 4);
    printf("Mapper Type: %d\n", mapper_type);

    mapper->type = mapper_type;
    mapper->prg_data = rom->PRG_data;
    mapper->chr_data = rom->CHR_data;
    switch (mapper_type)
    {
    case 0:
        mapper->get_byte = mapper0_get_byte;
        mapper->set_byte = mapper0_set_byte;
        break;
    default:
        printf("mapper type not valid\n");
        free(mapper);
        return NULL;
    }
    return mapper;
}

void mapper_exit(mapper_t* mapper) {
    free(mapper->prg_data);
    free(mapper->chr_data);
    free(mapper);
}

uint8_t mapper_get_byte(mapper_t* mapper,uint16_t addr) {
    switch (mapper->type)
    {
    case 0:
        break;
    }
}

void mapper_set_byte(mapper_t* mapper, uint16_t addr, uint8_t value) {

}

uint8_t mapper_get_word(mapper_t*, uint16_t) {

}

void mapper_set_word(mapper_t*, uint16_t, uint8_t) {

}

size_t mapper_get_bytes(uint8_t* data, uint16_t addr, uint32_t len) {

}

size_t mapper_set_bytes(uint16_t addr, uint8_t* data, uint32_t len) {

}

uint8_t mapper0_get_byte(mapper_t* mapper, uint16_t addr) {
    if (addr >= 0x8000 && addr <= 0xFFFF) {
        return mapper->prg_data[addr % 0x8000];
    } else {
        return mapper->prg_data[addr];
    } 
}

uint8_t mapper0_set_byte(mapper_t *mapper, uint16_t addr, uint8_t value) {
    if (addr >= 0x8000 && addr <= 0xFFFF) {
        mapper->prg_data[(addr % 0x4000) + 0x8000] = value;
    } else {
        mapper->prg_data[addr] = value;
    }
}