#include <stdio.h>
#include "../cpu.h"
#include "../memory.h"
#include "../machine.h"
#include "../instruction.h"

void load_ram() {
   FILE *f = fopen("test.bin", "rb");
   if (!f) {
      printf("read test rom error\n");
      return 1;
   }
   uint8_t bin_data[64*1024] = {0};
   size_t n = fread(bin_data, 1, 64*1024, f);
   if (n != 64*1024) {
      printf("read test rom size not right!\n");
      return 1;
   }
   fclose(f);
   set_bytes(0, bin_data, 64*1024);
}

void main() {
   register_t* reg = get_reg();
   
   reg->PC = 0x400;
   reg->P = 0x24;
   reg->A = 0;
   reg->X = 0;
   reg->Y = 0;
   reg->SP = 0xFF;

   printf("loading rom.\n");
   load_ram(reg); 
   printf("testing\n");
   uint16_t last_pc = reg->PC;
   size_t count = 0;
   for (;;) {
      // fetch opcode
      uint8_t opcode = get_byte(reg->PC);
      // decode instruction
      instruction_t instruction = decode_instruction(opcode);
      if (strcmp(instruction.name, "NDF") == 0 || instruction.length == 0 || instruction.exec_func == NULL) {
         printf("failed to decode instruction, opcode[%02x] not found\n", opcode);
         return 1;
      }
      // fetch other parts of instruction
      uint8_t other_data[5] = {0};
      get_bytes(other_data, reg->PC + 1, instruction.length-1);
      // execute
      exector_arg_t arg = { .instr = instruction };
      memcpy(arg.other, other_data, sizeof(other_data));

      // print_asm(arg);
      // print_registers();

      int res_code = instruction.exec_func(arg);
      if (res_code != 0) {
         printf("error result code: %d\n", res_code);
         return;
      }
      // stop
      if (reg->PC == last_pc) break; else last_pc = reg->PC;

      count ++;
   }

   if (reg->PC == 0x3469) {
      printf("\033[32mrun %ld instructions, all instructions test passed!\n\033[0m", count);
   } else {
      printf("\033[31mrun %ld instructions, failed on PC[%04x]\n\033[0m", count, reg->PC);
   }
}