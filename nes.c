#include "nes.h"
#include "instruction.h"
#include "rom.h"
#include "mapper.h"


void print_mem(uint16_t addr, uint16_t len) {
    printf("mem[0x%02X-0x%02X]: ", addr, addr+len);
    for (uint16_t i=0; i < len; i++) {
        printf("%02X ", get_byte(addr+i));
    }
    printf("\n");
}

int save_argb32_to_ppm(const char *filename,
                       const uint32_t *rgba32,  // 每像素 uint32_t
                       int width, int height)
{
    FILE *fp = fopen(filename, "wb+");
    if (!fp) return -1;

    fprintf(fp, "P6\n%d %d\n255\n", width, height);

    int count = width * height;
    for (int i = 0; i < count; ++i) {
        uint32_t pixel = rgba32[i];

        // 假设输入是 ARGB (小端常见)
        uint8_t r = (pixel >> 16) & 0xFF;
        uint8_t g = (pixel >> 8) & 0xFF;
        uint8_t b = pixel & 0xFF;

        fputc(r, fp);
        fputc(g, fp);
        fputc(b, fp);
    }

    fclose(fp);
    return 0;
}

void debug_print(exector_arg_t arg) {
    switch (arg.instr.mode)
    {
    case REGISTER:
        printf("%s            ", arg.instr.name);
        break;
    case IMMEDIATE:
        printf("%s #$%02X       ", arg.instr.name, arg.other[0]);
        break;
    case IMPLIED:
        printf("%s            ", arg.instr.name);
        break;
    case RELATIVE:
        printf("%s %02X         ", arg.instr.name, arg.other[0]);
        break;
    case ZERO_PAGE:
        printf("%s $%02X        ", arg.instr.name, arg.other[0]);
        break;
    case ZERO_PAGE_X:
        printf("%s $%02X, X     ", arg.instr.name, arg.other[0]);
        break;
    case ZERO_PAGE_Y:
        printf("%s $%02X, Y     ", arg.instr.name, arg.other[0]);
        break;
    case ABSOLUTE:
        printf("%s $%02X%02X      ", arg.instr.name, arg.other[1], arg.other[0]);
        break;
    case ABSOLUTE_X:
        printf("%s $%02X%02X, X   ", arg.instr.name, arg.other[1], arg.other[0]);
        break;
    case ABSOLUTE_Y:
        printf("%s $%02X%02X, Y   ", arg.instr.name, arg.other[1], arg.other[0]);
        break;
    case INDIRECT:
        printf("%s ($%02X%02X)    ", arg.instr.name, arg.other[1], arg.other[0]);
        break;
    case INDIRECT_X:
        printf("%s ($%02X, X)   ", arg.instr.name, arg.other[0]);
        break;
    case INDIRECT_Y:
        printf("%s ($%02X), Y   ", arg.instr.name, arg.other[0]);
        break;
    default:
    }
    reg_t *reg = get_reg();
    printf("  PC: 0x%04X SP: 0x%02X P: 0x%02X A: 0x%02X X: 0x%02X Y: 0x%02X",
        reg->PC, reg->SP, reg->P, reg->A, reg->X, reg->Y);
    printf("\n");
}

nes_t* nes_init(char *rom_file,  int screen_width, int screen_height) {
    Rom *rom = load_nes_rom(rom_file);
    if (!rom) {
        printf("failed to load rom\n");
        return NULL;
    }

    mapper_t *mapper = mapper_init(rom);
    if (!mapper) {
        printf("failed to init mapper\n");
        return NULL;
    }

    APU *apu = apu_init(); 
    if (!apu) {
        printf("failed to init apu\n");
        return NULL;
    }

    PPU *ppu = ppu_init(rom->CHR_data);
    if (!ppu) {
        printf("failed to init ppu\n");
        return NULL;
    }

    memory_t *mem = mem_init(mapper, ppu);
    if (!mem) {
        printf("failed to init memory\n");
        return NULL;
    }


    Video *video = video_create(screen_width, screen_height, "NES EMULATOR"); 
    if (!video) {
        printf("failed to init video\n");
        return NULL;
    }
    cpu_reset();
    nes_t *nes = malloc(sizeof(nes_t));
    nes->cpu = get_cpu();
    nes->mem = mem;
    nes->apu = apu;
    nes->ppu = ppu;
    nes->video = video;
    nes->audio = NULL;
    nes->screen_height = screen_height;
    nes->screen_width = screen_width;
    return nes;
}

void nes_reset(nes_t *nes) {
    
}

void nes_exit(nes_t *nes) {
    mem_exit();
    ppu_exit(nes->ppu);
    free(nes);
}

// run one instruction
// return cycles costed
int cpu_step(nes_t *nes) {
    reg_t* reg = &nes->cpu->reg;
    if (reg->PC == RESET_VECTOR) {
        reg->PC = get_word(RESET_VECTOR); 
        return 1;
    } else if(reg->PC == INTERRUPT_VECTOR_BRK) {
        return 1;
    } else if(reg->PC == INTERRUPT_VECTOR_NMI) {
        return 1;
    }
    // fetch opcode
    uint8_t opcode = get_byte(reg->PC);
    // decode instruction
    instruction_t instruction = decode_instruction(opcode);
    if (strcmp(instruction.name, "NDF") == 0) {
        printf("failed to decode instruction, opcode[%x] not found\n", opcode);
        return -1;
    }
    // fetch other parts of instruction
    uint8_t other_data[5] = {0};
    get_bytes(other_data, reg->PC + 1, instruction.length-1);
    // execute
    exector_arg_t arg = { .instr = instruction };
    memcpy(arg.other, other_data, sizeof(other_data));

    // debug_print(arg);
    // if (strcmp(arg.instr.name, "JSR") == 0) {
    //     return -1;
    // }

    return instruction.exec_func(arg);
}

void nes_adapt_screen(
    uint32_t *out_buffer, int out_width, int out_height, 
    uint32_t *in_buffer, int in_width, int in_height) {
    int width_scale = out_width / in_width;
    int height_scale = out_height / in_height;
    for(int i=0; i<in_width*in_height; i++) {
        for(int j=0; j<height_scale; j++) {
            for(int k=0; k<width_scale; k++) {
                int idx = (i / in_width) * height_scale * (in_width * width_scale) + (j * in_width * width_scale) + (i % in_width) * width_scale + k;
                out_buffer[idx] = in_buffer[i];
            }
        }
    }
}

void nes_run(nes_t *nes) {
    uint32_t frame_count = 0;
    uint32_t *screen_buffer = malloc(nes->screen_width * nes->screen_height * sizeof(uint32_t));
    while(1) {
        int cpu_cycles = cpu_step(nes);
        if (cpu_cycles < 0) {
            printf("cpu occur error, PC:0x%04x\n", get_reg()->PC);
            return;
        }

        // if (get_reg()->PC == 0x814A) {
        //     return -1;
        // }

        // for (int i=0; i<cpu_cycles; i++) {
        //     apu_step(nes->apu, NULL, nes->mem, 0);
        // }

        for (int i=0; i<3*cpu_cycles; i++) {
            ppu_step(nes->ppu, nes->cpu);
            if (nes->cpu->NMI == 1) {
                NMI();
                break;
            }
        }
            if (nes->cpu->NMI == 1) {
                set_NMI(0);
                continue;
            }
        if (get_reg()->PC == 0x90cc) {
            printf("=======InitializeMemory========\n");
        } else if (get_reg()->PC == 0x8e19) {
            printf("=======InitializeNameTables======\n");
        } else if (get_reg()->PC == 0x8046) {
            printf("=======InitializeSprites========\n");
        }
        // render frame
        if (nes->ppu->frame_done) {
            uint32_t temp[PPU_SCREEN_WIDTH*PPU_SCREEN_HEIGHT];
            fetch_frame(&temp, nes->ppu);
            nes_adapt_screen(screen_buffer, nes->screen_width, nes->screen_height, temp, PPU_SCREEN_WIDTH, PPU_SCREEN_HEIGHT);
            printf("draw %d frame\n", ++frame_count);
            video_render_line(nes->video, screen_buffer);
            if(frame_count == 40) {
                uint32_t palette[32];
                uint32_t palette_draw[256*64];
                fetch_palette(palette, nes->ppu);
                for(int n=0; n<64; n++)
                    for(int i=0; i<32; i++)
                        for(int j=0; j<8;j ++)
                            palette_draw[n*256+i*8+j] = palette[i];
                char filename[20];
                sprintf(filename, "palette-%03d.ppm", frame_count);
                save_argb32_to_ppm(filename, palette_draw, 256, 64);
            }
            // if(frame_count == 10) {
            //     printf("\n--------------PPU::pattern--------------------\n");
            //     uint8_t pattern[128*128];
            //     uint32_t tmp[128*128];
            //     fetch_pattern_table(pattern, nes->ppu, 1);
            //     for(int i=0; i<128*128; i++) {
            //         if(pattern[i] == 0) tmp[i] = 0x00FFFFFF; else temp[i] =0xFF000000;
            //     }
            //     char filename[20];
            //     sprintf(filename, "pattern-%03d.ppm", frame_count);
            //     save_argb32_to_ppm(filename, tmp, 128, 128);
            //     printf("\n------------------------------------------------\n");
            // }
            if(frame_count == 40) {
                printf("================PPU::SPRITES=================\n");
                uint32_t sprites[PPU_SCREEN_HEIGHT*PPU_SCREEN_WIDTH];
                fetch_sprites(nes->ppu, sprites);
                char filename[20];
                sprintf(filename, "sprite-%03d.ppm", frame_count);
                save_argb32_to_ppm(filename, sprites, PPU_SCREEN_WIDTH, PPU_SCREEN_HEIGHT);
                printf("=============================================\n");
            }
            if(frame_count == 666) {
                char filename[20];
                sprintf(filename, "snapshot-%03d.ppm", frame_count);
                save_argb32_to_ppm(filename, screen_buffer, nes->screen_width, nes->screen_height);
            }
            // SDL_Delay(500);
        }
        InputState status = video_poll_events(nes->video);
        if(status.quit) {
            return -1;
        }
    }
    free(screen_buffer);
}