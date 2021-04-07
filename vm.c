#include <stdio.h>
#include <stdlib.h>
#include "vm.h"
#include "parser.h"

#define MEM_SIZE 256
#define NUM_REG 8
#define SP 6 // references the bottom of the stack
#define PC 7 // references the current opcode, i.e the instruction struct
#define FP 5 // points to start of current frame
#define GEN 4 // for random shit

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


BYTE get_stk_sym_addr(BYTE *registers, BYTE stk_sym) {
	return registers[FP] + stk_sym;
}


void store_reg(BYTE *registers, BYTE reg, BYTE val) {
	registers[reg] = val;
}

void store_stk(BYTE *registers, BYTE *ram, BYTE addr, BYTE val) {
	if (registers[SP] < addr) {
		ram[addr] = val;
	}
}

BYTE access_stk_sym(BYTE *registers, BYTE *ram, BYTE stk_sym) {
		return ram[registers[FP] - stk_sym];
}


BYTE deref_ptr(BYTE *registers, BYTE *ram, BYTE stk_sym) {
	return ram[access_stk_sym(registers, ram, stk_sym)];
}
void call(BYTE *registers, BYTE *ram, BYTE label) {
	push(registers, ram, registers[FP]);
	push(registers, ram, registers[PC]);
	registers[PC] = instr_map[label];

}
void ret(BYTE *registers, BYTE *stk) {
	registers[GEN] = registers[FP] - registers[SP];

	while (registers[GEN] > 0) {
		pop(registers, stk);
		registers[GEN] -= 1;
	}

	registers[PC] = pop(registers, stk);
	registers[FP] = pop(registers, stk);
}

void ref(BYTE *registers, BYTE *ram, BYTE A_type, BYTE A, BYTE B) {
	if (A_type == REG) {
		store_reg(registers, A, get_stk_sym_addr(registers, B));
	} else {
		store_stk(registers, ram, A, get_stk_sym_addr(registers, B));
	}
}

void add(BYTE *registers, BYTE *ram, BYTE A, BYTE B) {
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

