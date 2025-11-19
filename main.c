#include <stdio.h>
#include "nes.h"

void main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("need arguments\n");
        return 1;
    }

    nes_t *nes = nes_init(argv[1], 1024, 960);
    if (!nes) {
        printf("failed to init nes\n");
        return 1;
    } 
    printf("nes run...\n");
    nes_run(nes);
}

