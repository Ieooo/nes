#ifndef __PPU_H__
#define __PPU_H__

#include "cpu.h"
#include <stdint.h>
#include <stdbool.h>

#define PPU_SCREEN_WIDTH  256
#define PPU_SCREEN_HEIGHT 240

#define NAME_TABLE_ADDR     0x2000
#define PATTERN_TABLE_ADDR  0x0000

// ppu register mapping address
#define PPU_CTRL     0x2000
#define PPU_MASK     0x2001
#define PPU_STATUS   0x2002
#define PPU_OAM_ADDR 0x2003
#define PPU_OAM_DATA 0x2004
#define PPU_SCROLL   0x2005
#define PPU_ADDR     0x2006
#define PPU_DATA     0x2007
#define PPU_DMC      0x4014



/*
    v/t register:
    yyy NN YYYYY XXXXX
    ||| || ||||| +++++-- coarse X (tile X, 0–31)
    ||| || +++++-------- coarse Y (tile Y, 0–29)
    ||| ++-------------- nametable select (bit10=x, bit11=y)
    +++----------------- fine Y (tile 0–7)
*/

typedef struct {
    // internal register
    uint16_t v;  // coarse_x(bit0-bit4), coarse_y(bit5-bit9), nameatble_select(bit10-bit11), fine_y(bit12-bit14) 
    uint16_t t;  // tempory vram address / coarse_x + coarse_y + fine_y, 15-bit
    uint8_t  x;  // fine x scroll, 3-bit
    bool     w;  // write lock

    uint16_t write_v;  // copy of v when v is written, use to set vram
    uint16_t render_v; // copy of v when v is written, use to render

    // internal ram
    uint8_t oam[256];
    uint8_t secondary_oam[32];
    uint8_t secondary_oam_src_idx[8];
    // for Name Table, Pattern Table, Palette
    uint8_t vram[16*1024];
    // [7]: vblank flag
    // [6]: sprite 0 hit flag
    // [5]: sprite overflow flag
    // [4-0]: reserved
    uint8_t ppu_status;
    // [7-0]: oam addr
    uint8_t ppu_oam_addr;
    // [7]: blue color
    // [6]: green color
    // [5]: red color
    // [4]: show sprites, 0=close, 1=open
    // [3]: show background, 0=close, 1=open
    // [2]: show left 8 column of Sprite, 0=not show, 1=show
    // [1]: show left column of background, 0=not, 1=show
    // [0]: reserved
    uint8_t ppu_mask;
    // [7]: NMI enable
    // [6]: PPU master/slave
    // [5]: spirte size
    // [4]: backgroud pattern table address, 0=$0000, 1=$1000
    // [3]: sprite pattern table address, 0=$0000, 1=$1000
    // [2]: vram address increment per cpu read/write, 0=+1, 1=+32
    // [1-0]: base nametable address, 0=$2000, 1=$2400, 2=$2800, 3=$2C00
    uint8_t ppu_ctrl;
    uint8_t ppu_oam_dma;

    uint16_t scanline;
    uint16_t cycle;
    bool     scroll_write_flag;
    bool     vram_write_flag;

    // store pixel color data when frame is rendering
    uint8_t frame_buffer[PPU_SCREEN_WIDTH * PPU_SCREEN_HEIGHT];  
    // store pixel color data when frame rendering complete
    uint8_t frame_data[PPU_SCREEN_WIDTH * PPU_SCREEN_HEIGHT];
    bool frame_done;
} PPU;

PPU* ppu_init(uint8_t*);
void ppu_exit(PPU*);
int ppu_step(PPU*, CPU*);
void ppu_write_register(PPU*, uint16_t, uint8_t);
uint8_t ppu_get_register(PPU*, uint16_t);
void fetch_frame(uint32_t* buffer, PPU *ppu);

void fetch_palette(uint32_t* buffer, PPU *ppu);
void fetch_pattern_table(uint8_t* buffer, PPU *ppu, uint8_t table_idx);
void get_draw_coord(PPU*, int*, int*);
void fetch_sprites(PPU *ppu, uint32_t *buffer);

#endif