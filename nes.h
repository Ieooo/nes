#ifndef __NES_H__
#define __NES_H__

#include <stdint.h>
#include "memory.h"
#include "cpu.h"
#include "apu.h"
#include "ppu.h"
#include "video.h"
#include "audio.h"

typedef struct{
    CPU      *cpu;
    memory_t *mem;
    PPU      *ppu;
    APU      *apu;
    Video    *video;
    Audio    *audio;

    int screen_width, screen_height;
} nes_t;


nes_t* nes_init(char*, int, int);
void nes_exit(nes_t*);
void nes_run(nes_t*);

#endif