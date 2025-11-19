#include "ppu.h"
#include <stdio.h>
#include <string.h>

static const uint32_t nes_palette[64] = {
    0xFF7C7C7C, 0xFF0000FC, 0xFF0000BC, 0xFF4428BC, 0xFF940084, 0xFFA80020, 0xFFA81000, 0xFF881400,
    0xFF503000, 0xFF007800, 0xFF006800, 0xFF005800, 0xFF004058, 0xFF000000, 0xFF000000, 0xFF000000,
    0xFFBCBCBC, 0xFF0078F8, 0xFF0058F8, 0xFF6844FC, 0xFFDC00BC, 0xFFE40058, 0xFFF81800, 0xFFF87800,
    0xFFFCBC00, 0xFFFCBC00, 0xFFFC9400, 0xFFFC9400, 0xFFFC9400, 0xFF000000, 0xFF000000, 0xFF000000,
    0xFFF8F8F8, 0xFF3CBCFC, 0xFF6888FC, 0xFF9878F8, 0xFFF878F8, 0xFFF85898, 0xFFF87858, 0xFFFCA044,
    0xFFF8B800, 0xFFB8F818, 0xFF58D854, 0xFF58F898, 0xFF00E8D8, 0xFF000000, 0xFF000000, 0xFF000000,
    0xFFFFFFFF, 0xFFA4E4FC, 0xFFB8B8F8, 0xFFD8B8F8, 0xFFF8B8F8, 0xFFF8A4C0, 0xFFF0D0B0, 0xFFFCE0A8,
    0xFFF8D878, 0xFFD8F878, 0xFFB8F8B8, 0xFFB8F8D8, 0xFF00FCFC, 0xFFF8D8F8, 0xFF000000, 0xFF000000
};

typedef struct {
    uint8_t pixel;
    bool is_sprite0;
    bool priority;
    uint8_t palette;
} sp_res_t;

typedef struct {
    uint8_t pixel;
    uint8_t palette;
} bg_res_t;


int ppu_debug_compare_sprite_pixel(PPU *ppu);
void ppu_evalute_sprites(PPU *ppu);

PPU* ppu_init(uint8_t* chr_data) {
    PPU *ppu = malloc(sizeof(PPU));
    memcpy(ppu->vram, chr_data, 8*1024);
    // cycle start from 1.
    ppu->cycle = 1;
    ppu->scanline = 0;
    return ppu;
}

void ppu_exit(PPU *ppu) {
    free(ppu);
}

int ppu_step(PPU *ppu, CPU *cpu) {
    // render cycles
    if (ppu->scanline < 240 && ppu->cycle >= 1 && ppu->cycle <= 256) {
        if((ppu->cycle - 1) % 8 == 0) {
            if ((ppu->render_v & 0x001F) == 0x001F) {     // when coarseX = 0x1F, flip bit10
                ppu->render_v &= ~0x001F;
                ppu->render_v ^= 0x0400;
            } else {
                ppu->render_v += 1;                   // coarseX increase
            }
        }
        if (ppu_debug_compare_sprite_pixel(ppu)) {
            sleep(1);
        }
        render_pixel(ppu);
        if (ppu->scanline==0) {
            int x, y = 0;
            get_draw_coord(ppu, &x, &y);
            // printf("============= DRAW POINT(%d,%d)============\n", x, y);
        }
    }

    if (ppu->cycle == 257) {
        if ((ppu->render_v & 0x7000) != 0x7000) {
            ppu->render_v += 0x1000;
        } else {
            ppu->render_v &= ~0x7000;                  // fine Y = 0
            int y = (ppu->render_v & 0x03E0) >> 5;     // coarse Y
            if (y == 29) {
                ppu->render_v &= ~0x03E0;
                ppu->render_v ^= 0x0800;          // flip nametable Y (bit11)
            } else if (y == 31) {
                ppu->render_v &= ~0x03E0;
            } else {
                ppu->render_v += 0x20;
            }
        }
    }

    if(ppu->cycle == 261) {
        ppu->render_v = (ppu->render_v & ~0x041F) | (ppu->v & 0x041F); // copy coarseX, nametableX
    }

    // Vblank cycles
    if (ppu->scanline == 241 && ppu->cycle == 1) {
        // set vblank flag($2002, bit7)
        ppu->ppu_status |= 0x80;
        // clear sprite 0 hit
        ppu->ppu_status &= ~0x40;
        // clear sprite overflow
        ppu->ppu_status &= ~0x20;
        if(ppu->ppu_ctrl & 0x80) {
            // printf("======================== PPU NMI =========================\n");
            // NMI when NMI enable
            cpu->NMI = 1;
        }
    }
    if (ppu->cycle == 257) {
        ppu_evalute_sprites(ppu);
    }
    if (ppu->scanline == 261 && ppu->cycle == 1) {
        // clear vblank flag($2002, bit7)
        ppu->ppu_status &= 0x7F;
    }

    // pre-render cycles
    if (ppu->cycle == 341) {
        ppu->cycle = 1;
        ppu->scanline++;

        if (ppu->scanline == 261) {
            ppu->render_v = (ppu->render_v & ~0x7BE0) | (ppu->v & 0x7BE0);   // copy coarsey, fineY from v to t when every scanline start
            ppu->scanline = 0;
            ppu->frame_done = true;
            // copy complete frame from temp to frame
            memcpy(ppu->frame_data, ppu->frame_buffer, PPU_SCREEN_HEIGHT * PPU_SCREEN_WIDTH);
        }
    } else {
        ppu->cycle ++;
    }
}

void ppu_write_register(PPU *ppu, uint16_t addr, uint8_t value) {
    switch (addr)
    {
    case PPU_ADDR:
        if (!ppu->vram_write_flag) {
            ppu->t = (uint16_t)(value & 0x3F) << 8;   // high byte
        } else {
            ppu->t = (ppu->t & 0xFF00) | value;  // low byte
            ppu->v = ppu->t;  // copy to v
            ppu->write_v = ppu->v;
            // if(ppu->write_v >= 0x3F00 && ppu->write_v <= 0x3f1F) {
            //     printf("===========PPU SET VRAM ADDR: %02x===========\n", ppu->write_v);
            //     sleep(1);
            // }
        }
        ppu->vram_write_flag = !ppu->vram_write_flag;
        break;
    case PPU_SCROLL:
        if (!ppu->scroll_write_flag) {
            // update coarseX in t and fineX in x
            ppu->t = (ppu->t & 0xFFE0) | (value & 0x1F);    // coarseX [0-4]
            ppu->x = (value >> 5) & 0x07;                   // fineX [5-7]
        } else {
            // update coarseY fineY in t
            ppu->t = ppu->t & 0xFC1F | ((uint16_t)(value & 0xF8) << 2);  // coarseY, value[3-7]
            ppu->t = ppu->t & 0x0FFF | ((uint16_t)(value & 0x07) << 12); // fineY, value[0-2]
        }
        ppu->scroll_write_flag = !ppu->scroll_write_flag;
        break;
    case PPU_CTRL:
        ppu->ppu_ctrl = value;
        break;
    case PPU_DATA:
        if(ppu->write_v == 0x3F10 ||
            ppu->write_v == 0x3F14 ||
            ppu->write_v == 0x3F18 ||
            ppu->write_v == 0x3F1C) {
                ppu->vram[0x3F00] = value;
                ppu->vram[ppu->write_v] = value;
        } else if(ppu->write_v >= 0x2000 && ppu->write_v <= 0x2FFF) {
            ppu->vram[(ppu->write_v%0x0800) + 0x2000] = value;
            ppu->vram[(ppu->write_v%0x0800) + 0x2800] = value;
        } else {
            ppu->vram[ppu->write_v] = value;
        }
        ppu->write_v += ((ppu->ppu_ctrl & 0x04) == 0 ? 1 : 32);
        break;
    case PPU_MASK:
        ppu->ppu_mask = value;
        break;
    case PPU_OAM_ADDR:
        ppu->ppu_oam_addr = value;
        break;
    case PPU_OAM_DATA:
        ppu->oam[ppu->ppu_oam_addr++] = value;
        break;
    case PPU_STATUS:
        ppu->ppu_status = value;
        break;
    default:
        break;
    }
}

uint8_t ppu_get_register(PPU *ppu, uint16_t addr) {
    switch (addr)
    {
    case PPU_ADDR:
        return 0;
    case PPU_SCROLL:
        return 0;
    case PPU_CTRL:
        return ppu->ppu_ctrl;
    case PPU_DATA:
        return 0;
    case PPU_MASK:
        return ppu->ppu_mask;
    case PPU_OAM_ADDR:
        return ppu->ppu_oam_addr;
    case PPU_OAM_DATA:
        return 0;
    case PPU_STATUS:
        uint8_t status = ppu->ppu_status;
        // clear vblank
        ppu->ppu_status &= ~0x80;
        return status;
    default:
        return 0;
    }
}

/*
    Sprite structure(4 byte),
    X,Y is left-top point of Sprite
    [0]: Y position
    [1]: tile index
    [2]: attributes
    [3]: X position
*/
void ppu_evalute_sprites(PPU *ppu) {
    uint8_t sprite_height = (ppu->ppu_ctrl & 0x20) == 0 ? 8 : 16;
    // clear, use 0xFF
    memset(ppu->secondary_oam, 0xFF, 32);
    ppu->ppu_status &= ~0x20;

    uint8_t sprite_count = 0;
    for(int i=0; i<256; i+=4) {
        uint8_t y = ppu->oam[i];
        if (y <= ppu->scanline && (y + sprite_height) > ppu->scanline) {
            if (sprite_count >= 8) {
                ppu->ppu_status |= 0x20;
                break;            
            }
            // copy to secondary oam
            memcpy(ppu->secondary_oam + sprite_count * 4, ppu->oam + i, 4);
            ppu->secondary_oam_src_idx[sprite_count] = i/4;
            sprite_count ++;
        }
    }
}

sp_res_t get_sprite_pixel(PPU *ppu) {
    sp_res_t res = {
        .palette = 0,
        .pixel = 0,
        .is_sprite0 = 0,
        .priority = 0,
    };
    
    uint8_t sprite_height = (ppu->ppu_ctrl & 0x20) ? 16 : 8;
    uint8_t found_sprite0 = 0;
    
    // 按正确的优先级顺序处理精灵（从高到低）
    for (int i = 0; i < 8; i++) {
        uint8_t y = ppu->secondary_oam[i*4 + 0];
        uint8_t tile_idx = ppu->secondary_oam[i*4 + 1];
        uint8_t attr = ppu->secondary_oam[i*4 + 2];
        uint8_t x = ppu->secondary_oam[i*4 + 3];

        if (y == 0xFF) continue;

        int x_in_sprite = ppu->cycle - 1 - x;  // 注意：cycle从1开始
        int y_in_sprite = ppu->scanline - y - 1;
        
        if (x_in_sprite < 0 || x_in_sprite > 7) continue;
        if (y_in_sprite < 0 || y_in_sprite >= sprite_height) continue;

        // 计算图案地址（你的计算基本正确，但需要检查细节）
        uint16_t pattern_addr;
        uint8_t fine_y;
        
        if (sprite_height == 8) {
            if (attr & 0x80) { // 垂直翻转
                fine_y = 7 - y_in_sprite;   
            } else {
                fine_y = y_in_sprite;
            } 
            uint16_t base = (ppu->ppu_ctrl & 0x08) ? 0x1000 : 0x0000; // 注意：应该是bit3
            pattern_addr = base + (uint16_t)tile_idx * 16;
        } else {
            // 8x16精灵模式
            if (attr & 0x80) { // 垂直翻转
                y_in_sprite = sprite_height - 1 - y_in_sprite;
            }
            
            uint8_t bank = (tile_idx & 0x01) ? 0x1000 : 0x0000;
            uint8_t real_tile_idx = tile_idx & 0xFE; // 清除最低位
            
            if (y_in_sprite < 8) {
                // 上半部分
                pattern_addr = bank + real_tile_idx * 16 + (y_in_sprite & 7);
            } else {
                // 下半部分
                pattern_addr = bank + (real_tile_idx + 1) * 16 + (y_in_sprite & 7);
            }
        }

        // 读取图案数据
        uint8_t plane0, plane1;
        if (sprite_height == 8) {
            plane0 = ppu->vram[pattern_addr + fine_y];
            plane1 = ppu->vram[pattern_addr + fine_y + 8];
        } else {
            plane0 = ppu->vram[pattern_addr];
            plane1 = ppu->vram[pattern_addr + 8];
        }

        // 水平翻转处理
        uint8_t bit_index;
        if (attr & 0x40) { // 水平翻转
            bit_index = 7 - x_in_sprite; // 正常时，最左边的像素对应最高位
        } else {
            bit_index = x_in_sprite; // 翻转时，最左边的像素对应最低位
        }
        uint8_t low = (plane0 >> (7 - bit_index)) & 1;  // 修正位提取
        uint8_t high = (plane1 >> (7 - bit_index)) & 1;
        uint8_t pixel_value = (high << 1) | low;

        if (pixel_value == 0) continue;

        res.palette = attr & 0x03;
        res.priority = (attr >> 5) & 1;
        res.pixel = pixel_value;
        res.is_sprite0 = (i == 0);
        return res;
    }
    return res;
}

bg_res_t get_backgroud_pixel(PPU *ppu) {
    // use nametalbe_select and coarse
    uint16_t nametable_base = NAME_TABLE_ADDR | (ppu->render_v & 0x0C00);
    uint16_t tile_addr = nametable_base + ((ppu->render_v >> 5) & 0x001F) * 32 + (ppu->render_v & 0x001F);
    uint8_t title_idx = ppu->vram[tile_addr];


    // one title use 16 bit, every title is 8*8 pixel, every pixel use 2bit, 
    // title_idx is index in title.
    uint16_t pattern_table_base = (ppu->ppu_ctrl & 0x10) ? 0x1000 : 0x0000;
    uint8_t fine_y = (ppu->render_v >> 12) & 0x0007;
    uint16_t title_address = pattern_table_base + (uint16_t)title_idx * 16;
    uint16_t pattern_address = title_address + fine_y;
    uint8_t bit_plane0 = ppu->vram[pattern_address + 0];
    uint8_t bit_plane1 = ppu->vram[pattern_address + 8];
    // 2 bit pixel consists of bit_plane0(low bit) and bit_plane1(high bit).
    uint8_t s = (ppu->cycle - 1 + ppu->x) % 8;
    uint8_t pixel_value = ((bit_plane1 >> (7 - s)) & 1) << 1 | ((bit_plane0 >> (7 - s)) & 1);

    // Attribute Table start = NameTable base + 0x3C0(960 bytes)
    uint16_t coarse_x = (ppu->render_v & 0x001F) + ((ppu->cycle - 1 + ppu->x) >> 3) & 0x1F;
    uint16_t coarse_y = (ppu->render_v >> 5) & 0x001F;
    uint16_t attr_table_address = nametable_base + 0x03C0 + ((coarse_y >> 2) << 3) + (coarse_x >> 2);
    uint8_t attr_byte = ppu->vram[attr_table_address];
    uint8_t shift = ((coarse_y & 0x02) ? 4 : 0) | ((coarse_x & 0x02) ? 2 : 0);
    uint16_t palette_bits = (attr_byte >> shift) & 0x03;

    bg_res_t out = {
        .palette = palette_bits,
        .pixel = pixel_value,
    };
    return out;
}


void render_pixel(PPU *ppu) {
    bg_res_t bg_pixel = get_backgroud_pixel(ppu);
    sp_res_t sprite_pixel = get_sprite_pixel(ppu);

    if(!(ppu->ppu_mask & 0x08)) bg_pixel.pixel = 0;
    if(!(ppu->ppu_mask & 0x10)) sprite_pixel.pixel = 0;

    if (ppu->cycle <= 8) {
        if(!(ppu->ppu_mask & 0x02)) bg_pixel.pixel = 0;
        if(!(ppu->ppu_mask & 0x04)) sprite_pixel.pixel = 0;
    }

    // set sprite hit flag
    if(sprite_pixel.is_sprite0 && bg_pixel.pixel != 0 && sprite_pixel.pixel != 0) {
        ppu->ppu_status |= 0x40;
    }

    uint16_t color_idx;
    if(sprite_pixel.pixel == 0 && bg_pixel.pixel == 0) {
        color_idx = 0x3F00;
    }else if(sprite_pixel.pixel == 0) {
        color_idx = 0x3F00 + bg_pixel.pixel + bg_pixel.palette * 4;
    } else if(bg_pixel.pixel == 0) {
        color_idx = 0x3F10 + sprite_pixel.pixel + sprite_pixel.palette * 4;
    } else {
        if(!sprite_pixel.priority)
            color_idx = 0x3F10 + sprite_pixel.pixel + sprite_pixel.palette * 4;
        else
            color_idx = 0x3F00 + bg_pixel.pixel + bg_pixel.palette * 4;
    }

    uint8_t color = ppu->vram[color_idx];
    int frame_buffer_idx = ppu->scanline * PPU_SCREEN_WIDTH + ppu->cycle - 1;
    ppu->frame_buffer[frame_buffer_idx] = color & 0x3F;
}


void fetch_frame(uint32_t* buffer, PPU *ppu) {
    for(int i=0; i<PPU_SCREEN_WIDTH * PPU_SCREEN_HEIGHT; i++) {
        buffer[i] = nes_palette[ppu->frame_data[i]];
    }
    // reset state
    ppu->frame_done = false;
}

void fetch_palette(uint32_t* buffer, PPU *ppu) {
    for (int i=0; i<32; i++) {
        uint8_t color_idx = ppu->vram[0x3F00 + i] & 0x3F;
        buffer[i] = nes_palette[color_idx];
    }
}

void fetch_pattern_table(uint8_t* buffer, PPU *ppu, uint8_t table_idx) {
    uint16_t pattern_base = 0;
    if(table_idx == 0) {
        pattern_base = 0;
    } else if (table_idx == 1) {
        pattern_base = 0x1000; 
    } else {
        return;
    }
    for (int i = 0; i < 256; i++) {
        int row = i / 16;
        int col = i % 16;
    
        int tile_base = pattern_base + i * 16;
    
        for (int y = 0; y < 8; y++) {
            uint8_t bit0 = ppu->vram[tile_base + y];
            uint8_t bit1 = ppu->vram[tile_base + y + 8];
    
            for (int x = 0; x < 8; x++) {
                uint8_t b0 = (bit0 >> (7 - x)) & 1;
                uint8_t b1 = (bit1 >> (7 - x)) & 1;
                uint8_t color = (b1 << 1) | b0;
    
                int idx = (row * 8 + y) * 128 + col * 8 + x;
                buffer[idx] = color;
            }
        }
    }
}


void get_draw_coord(PPU *ppu, int *x, int *y) {
    uint16_t coarse_x = ppu->render_v & 0x1F;
    uint16_t coarse_y = (ppu->render_v >> 5) & 0x1F;

    uint8_t fine_y = (ppu->render_v >> 12) & 0x7;
    uint8_t fine_x = ppu->x;
    // printf("coarseX:%d, coarseY:%d, fineX:%d, fineY:%d\n", coarse_x, coarse_y, fine_x, fine_y);
    *x = (coarse_x * 8 + fine_x) % 256;
    *y = coarse_y * 8 + fine_y;
}


void fetch_sprites(PPU *ppu, uint32_t *buffer) {
    memset(buffer, 0, PPU_SCREEN_WIDTH * PPU_SCREEN_HEIGHT * sizeof(uint32_t));

    uint8_t sprite_height = (ppu->ppu_ctrl & 0x20) ? 16 : 8;

    for(int i=0; i<64; i++) {
        uint8_t y = ppu->oam[i*4];
        uint8_t tile_idx = ppu->oam[i*4+1];
        uint8_t attr = ppu->oam[i*4+2];
        uint8_t x = ppu->oam[i*4+3];

        if(y >= PPU_SCREEN_HEIGHT) continue; // 超出屏幕

        for(int row=0; row<sprite_height; row++) {
            int tile_y = row;
            if(attr & 0x80) tile_y = sprite_height - 1 - row; // 垂直翻转

            uint16_t pattern_addr;
            if(sprite_height == 8) {
                uint16_t pattern_table_base = (ppu->ppu_ctrl & 0x80) ? 0x1000 : 0x0000;
                pattern_addr = pattern_table_base + tile_idx * 16 + tile_y;
            } else {
                uint16_t pattern_table_base = (tile_idx & 0x01) ? 0x1000 : 0x0000;
                uint8_t base_tile = tile_idx & 0xFE;
                if(tile_y < 8)
                    pattern_addr = pattern_table_base + base_tile * 16 + tile_y;
                else
                    pattern_addr = pattern_table_base + (base_tile + 1) * 16 + (tile_y - 8);
            }

            uint8_t plane0 = ppu->vram[pattern_addr];
            uint8_t plane1 = ppu->vram[pattern_addr + 8];

            for(int col=0; col<8; col++) {
                int tile_x = col;
                if(attr & 0x40) tile_x = 7 - col; // 水平翻转

                uint8_t low = (plane0 >> (7 - tile_x)) & 1;
                uint8_t high = (plane1 >> (7 - tile_x)) & 1;
                uint8_t pixel_value = (high << 1) | low;

                if(pixel_value == 0) continue; // 透明像素

                uint8_t palette_idx = attr & 0x03;
                uint16_t final_palette_idx = 0x3F10 + palette_idx * 4 + pixel_value;
                if ((final_palette_idx & 0x03) == 0) final_palette_idx = 0x3F00;
                uint8_t color = ppu->vram[final_palette_idx] & 0x3F;

                int screen_x = x + col;
                int screen_y = y + row;
                if(screen_x < 0 || screen_x >= PPU_SCREEN_WIDTH || screen_y < 0 || screen_y >= PPU_SCREEN_HEIGHT) continue;

                buffer[screen_y * PPU_SCREEN_WIDTH + screen_x] = nes_palette[color];
            }
        }
    }
}

int ppu_debug_compare_sprite_pixel(PPU *ppu) {
    uint8_t sprite_height = (ppu->ppu_ctrl & 0x20) ? 16 : 8;

    for(int s = 0; s < 8; s++) {
        uint8_t sec_y = ppu->secondary_oam[s*4 + 0];
        uint8_t sec_tile = ppu->secondary_oam[s*4 + 1];
        uint8_t sec_attr = ppu->secondary_oam[s*4 + 2];
        uint8_t sec_x = ppu->secondary_oam[s*4 + 3];

        if (sec_y == 0xFF) continue;

        // 如果 secondary_oam 里不在当前 cycle 水平范围，也跳过（和 get_sprite_pixel 一致）
        if(ppu->cycle < sec_x || ppu->cycle >= sec_x + 8) continue;

        // ---- 方案 A: 按你现有 get_sprite_pixel 的计算（原始实现） ----
        // tile_x/tile_y (直接)
        int gx = ppu->cycle - sec_x;
        int gy = ppu->scanline - sec_y;

        // apply flips like your get_sprite_pixel had (horizontal/vertical)
        if (sec_attr & 0x40) gx = 7 - gx;
        if (sec_attr & 0x80) gy = sprite_height - 1 - gy;

        // compute pattern_addr like get_sprite_pixel (handle 8/16)
        uint16_t pattern_addr_get;
        if(sprite_height == 8) {
            uint16_t base = (ppu->ppu_ctrl & 0x80) ? 0x1000 : 0x0000;
            pattern_addr_get = base + sec_tile * 16 + gy;
        } else {
            uint16_t base = (sec_tile & 1) ? 0x1000 : 0x0000;
            uint8_t base_tile = sec_tile & 0xFE;
            uint8_t tile_y_in_tile = gy & 7;
            if (gy < 8)
                pattern_addr_get = base + base_tile * 16 + tile_y_in_tile;
            else
                pattern_addr_get = base + (base_tile + 1) * 16 + tile_y_in_tile;
        }
        uint8_t p0_get = ppu->vram[pattern_addr_get & 0x3FFF];
        uint8_t p1_get = ppu->vram[(pattern_addr_get + 8) & 0x3FFF];
        uint8_t low_get = (p0_get >> (7 - gx)) & 1;
        uint8_t high_get = (p1_get >> (7 - gx)) & 1;
        uint8_t pv_get = (high_get << 1) | low_get;

        // ---- 方案 B: 直接用 OAM（full OAM 渲染）在同一 scanline 取得像素 ----
        // 这里用 secondary_oam_src_idx[s] 找到原始 OAM 条目（如果你有记录）
        int orig_index = -1;
        if(ppu->secondary_oam_src_idx != NULL) orig_index = ppu->secondary_oam_src_idx[s];
        // 如果没有 src 索引，则尝试从 secondary_oam 的数据去匹配原始 OAM
        if(orig_index < 0) {
            // brute-force find in ppu->oam where tile,x,y match (first match)
            for(int k = 0; k < 64; k++) {
                if(ppu->oam[k*4+0] == sec_y && ppu->oam[k*4+1] == sec_tile &&
                   ppu->oam[k*4+2] == sec_attr && ppu->oam[k*4+3] == sec_x) {
                    orig_index = k;
                    break;
                }
            }
        }

        if(orig_index < 0) {
            printf("DEBUG: cannot find original OAM entry for secondary index %d\n", s);
            continue;
        }

        // 从原始 OAM 按照“直接绘制 sprite”方式获取同一 scanline 的像素
        uint8_t oam_y = ppu->oam[orig_index*4 + 0];
        uint8_t oam_tile = ppu->oam[orig_index*4 + 1];
        uint8_t oam_attr = ppu->oam[orig_index*4 + 2];
        uint8_t oam_x = ppu->oam[orig_index*4 + 3];

        // compute row in sprite relative to its top (the direct renderer likely used the same y)
        int dy = ppu->scanline - oam_y;
        if(dy < 0 || dy >= sprite_height) {
            // not visible (shouldn't happen if secondary_oam was filled correctly)
            continue;
        }

        int rx = ppu->cycle - oam_x;
        if(rx < 0 || rx >= 8) continue;

        // direct renderer typically does:
        int dx = rx;
        int row = dy;
        if(oam_attr & 0x40) dx = 7 - dx;
        if(oam_attr & 0x80) row = sprite_height - 1 - row;

        uint16_t pattern_addr_direct;
        if(sprite_height == 8) {
            uint16_t base = (ppu->ppu_ctrl & 0x80) ? 0x1000 : 0x0000;
            pattern_addr_direct = base + oam_tile * 16 + row;
        } else {
            uint16_t base = (oam_tile & 1) ? 0x1000 : 0x0000;
            uint8_t base_tile = oam_tile & 0xFE;
            uint8_t tile_y_in_tile = row & 7;
            if (row < 8)
                pattern_addr_direct = base + base_tile * 16 + tile_y_in_tile;
            else
                pattern_addr_direct = base + (base_tile + 1) * 16 + tile_y_in_tile;
        }
        uint8_t p0_dir = ppu->vram[pattern_addr_direct & 0x3FFF];
        uint8_t p1_dir = ppu->vram[(pattern_addr_direct + 8) & 0x3FFF];
        uint8_t low_dir = (p0_dir >> (7 - dx)) & 1;
        uint8_t high_dir = (p1_dir >> (7 - dx)) & 1;
        uint8_t pv_dir = (high_dir << 1) | low_dir;

        // Compare
        if(pv_get != pv_dir || pattern_addr_get != pattern_addr_direct || gx != dx || gy != row) {
            printf("MISMATCH sprite orig_idx=%d sec_slot=%d\n", orig_index, s);
            printf(" get: pattern=0x%04X tile_x=%d tile_y=%d pv=%d\n", pattern_addr_get, gx, gy, pv_get);
            printf(" dir: pattern=0x%04X tile_x=%d tile_y=%d pv=%d\n", pattern_addr_direct, dx, row, pv_dir);
            return orig_index + 1; // 非0 表示发现不一致（返回 orig index+1）
        }
    }

    return 0;
}