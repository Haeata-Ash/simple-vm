#include <stdio.h>
#include <stdlib.h>
#include "vm.h"
#include "parser.h"

#define MEM_SIZE 256
#define NUM_REG 8
#define SP 6
#define PC 7
#define FP 5
#define GEN 4

struct PMEM {
	struct Function *functions;
	int num_functions;
};

struct Memory {
	struct PMEM *pmem;
	BYTE *ram;
	BYTE *registers;
};

int main(int argc, char **argv) {
	struct PMEM pmem;
	struct Function functions[MAX_FUNC];
	pmem.functions = &functions[0];
	BYTE ram[MEM_SIZE];
	BYTE registers[NUM_REG];
	BYTE *ptr_ram = &ram[0];
	BYTE *ptr_reg = &registers[0];

	//read in program to pmem
	char *file = argv[1];
	FILE *fp = fopen(file, "rb");
	pmem.num_functions = parse(fp, pmem.functions);

	//set stack pointer to 255
	registers[SP] = 0xFF;
	
	//set program counter to 0
	registers[PC] = 0x00;


}

int run(struct PMEM pmem, BYTE *ram, BYTE *registers) {

	
	return 1;
}

void ref(BYTE *registers, BYTE *stk, BYTE A_type, BYTE A, BYTE B) {
	switch (A_type) {
		case (REG) {

		}
	}

}

void store_reg(BYTE *registers, BYTE reg, BYTE val) {
	registers[reg] = val;
}

void store_stk_sym(BYTE *registers, BYTE *ram, BYTE stk_sym, BYTE val) {
	if (registers[FP] - stk_sym > registers[SP]) {
		ram[registers[FP] - stk_sym] = val;
	}
}

BYTE access_stk_sym(BYTE *registers, BYTE *ram, BYTE stk_sym) {
	if (registers[FP] - stk_sym > registers[SP]) {
		return ram[registers[FP] - stk_sym];
	}
}

BYTE deref_ptr()


void add(BYTE *registers, BYTE A, BYTE B) {
	store_reg(registers, A, registers[A] + registers[B]);
 }
void print(BYTE *storage, BYTE addr) {
	printf("%d\n", storage[addr]);
}

void not(BYTE *registers, BYTE reg_addr) {
	store_reg(registers, reg_addr, ~(registers[reg_addr]));
}

void equ(BYTE reg_addr, BYTE *ptr_reg) {
	if (ptr_reg[reg_addr] == 0) {
		ptr_reg[reg_addr] = 0x01;
	} else {
		ptr_reg[reg_addr] = 0x00;
	}
}

BYTE current_func(BYTE *ptr_reg) {
	return ptr_reg[PC] & mask(LEN_OPCODE);
}

BYTE current_inst(BYTE *ptr_reg) {
	return (ptr_reg[PC] >> LEN_OPCODE) & mask(LEN_INST);
}




void inc_PC(BYTE *ptr_reg, struct Function *pmem) {
	ptr_reg[GEN] = ptr_reg[PC] & mask(LEN_OPCODE);

	if (pmem[ptr_reg[GEN]].num_inst > ((ptr_reg[PC] >> LEN_OPCODE) & mask(LEN_INST))) {
		ptr_reg[GEN] = ptr_reg[PC] >> LEN_OPCODE;
		ptr_reg[GEN] += 0x01;
		ptr_reg[GEN] <<= LEN_OPCODE;
		ptr_reg[GEN] += ptr_reg[PC] & mask(LEN_OPCODE);
		ptr_reg[PC] = ptr_reg[GEN];
	else {

	}
			}
}

void set_PC(BYTE *ptr_reg, BYTE addr) {
	ptr_reg[PC] = addr;
}

