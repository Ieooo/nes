#include "machine.h"
#include "instruction.h"

void print_mem(uint16_t addr, uint16_t len) {
    printf("mem[0x%02X-0x%02X]: ", addr, addr+len);
    for (uint16_t i=0; i < len; i++) {
        printf("%02X ", get_byte(addr+i));
    }
    printf("\n");
}

void run() {
    cpu_reset();
    mem_init();
    register_t* reg = get_reg();
    for (;;) {
        // fetch opcode
        uint8_t opcode = get_byte(reg->PC);
        // decode instruction
        instruction_t instruction = decode_instruction(opcode);
        if (instruction.name == "NDF") {
            printf("failed to decode instruction, opcode[%x] not found\n", opcode);
            return 1;
        }
        // fetch other parts of instruction
        uint8_t other_data[5] = {0};
        get_bytes(other_data, reg->PC + 1, instruction.length-1);
        // execute
        exector_arg_t arg = { .instr = instruction };
        memcpy(arg.other, other_data, sizeof(other_data));

        print_asm(arg);
        //print_registers();

        int res_code = instruction.exec_func(arg);
        if (res_code != 0) {
            printf("error execute code: %d\n", res_code);
            return;
        }
        sleep(1);
    }
    printf("exit\n");
}