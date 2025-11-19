#include "rom.h"
#include "stdio.h"

Rom* load_nes_rom(char* filename) {
    FILE* fp = fopen(filename, "rb");
    if (!fp) {
        return NULL;
    }

    rom_header header;
    size_t n = fread(&header, sizeof(rom_header), 1, fp);
    if (!n) {
        fclose(fp);
        return NULL;
    }
    printf("=========LOAD ROM=========\n");
    printf("magic:%s\n", header.magic);
    printf("PRG size:%d\n", header.PRG_rom_size);
    printf("CHR size:%d\n", header.CHR_rom_size);
    printf("TV type:%d\n", header.tv_type);
    printf("PRG ram size:%d\n", header.PRG_ram_size);
    printf("Flag6:0x%02X\n", header.flag6);
    printf("Flag7:0x%02X\n", header.flag7);
    printf("=========LOAD ROM=========\n");

    Rom *rom;
    uint8_t *trainer, *PRG_rom_data, *CHR_rom_data;
    rom = malloc(sizeof(Rom));
    if (!rom) {
        fclose(fp);
        return NULL;
    }
    rom->header = header;

    if (header.flag6 == 0x04) {
        trainer = malloc(512);
        if (!trainer) {
            fclose(fp);
            return NULL;
        }
        n = fread(trainer, sizeof(uint8_t), 512, fp);
        if (!n) {
            fclose(fp);
            return NULL;
        }
        rom->trainer = trainer;
    } else {
        rom->trainer = NULL;
    }

    PRG_rom_data = malloc(header.PRG_rom_size*16*1024);
    if (!PRG_rom_data) {
        fclose(fp);
        return NULL;
    }

    n = fread(PRG_rom_data, 1, header.PRG_rom_size*16*1024, fp);
    if (!n) {
        fclose(fp);
        return NULL;
    }
    rom->PRG_data = PRG_rom_data;

    CHR_rom_data = malloc(header.CHR_rom_size*8*1024);
    if (!CHR_rom_data) {
        fclose(fp);
        return NULL;
    }
    n = fread(CHR_rom_data, 1, header.CHR_rom_size*8*1024, fp);
    if (!n) {
        fclose(fp);
        return NULL;
    }
    rom->CHR_data = CHR_rom_data;

    fclose(fp);
    return rom;
}