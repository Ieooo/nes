#ifndef __MAPPER_H__
#define __MAPPER_H__

#include <stdint.h>
#include <stdio.h>
#include "rom.h"

typedef struct mapper{
    uint8_t  type;
    uint8_t *prg_data;
    uint8_t *chr_data;
    // operations
    void (*set_byte)(struct mapper*,uint16_t, uint8_t);
    uint8_t (*get_byte)(struct mapper*, uint16_t);
} mapper_t;

mapper_t* mapper_init(Rom*);
void mapper_exit(mapper_t*);

#endif