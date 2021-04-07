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


int main(int argc, char **argv) {
	struct PMEM pmem;
	pmem.num_inst = 0;
	pmem.num_functions = 0;

	BYTE ram[MEM_SIZE];
	BYTE registers[NUM_REG];

	//read in program to pmem
	char *file = argv[1];
	FILE *fp = fopen(file, "rb");
	parse(fp, &pmem);

	//set stack pointer to 255
	registers[SP] = 0xFF;
	
	//set program counter to 0
	registers[PC] = 0x00;
	run(&pmem, &ram[0], &registers[0]);


}

int run(struct PMEM *pmem, BYTE *ram, BYTE *registers) {
	while (registers[PC] < pmem->num_inst) {

		switch(pmem->inst[registers[PC]].opcode) {
			case EQU:
				equ(pmem->inst[registers[PC]].args[1], registers);
				break;
			case NOT:
				not(registers, pmem->inst[registers[PC]].args[1]);
				break;
			case PRINT:
				print(registers, ram, pmem->inst[registers[PC]].args[0], pmem->inst[registers[PC]].args[1]);
				break;
			case ADD:
				add(registers, ram, pmem->inst[registers[PC]].args[1],pmem->inst[PC].args[3]);
				break;
			case REF:
				ref(registers, ram, pmem->inst[registers[PC]].args[0], pmem->inst[registers[PC]].args[1], pmem->inst[registers[PC]].args[3]);
				break;
			case RET:
				ret(registers, ram);
				break;
			case CAL:
				call(pmem, registers, ram, pmem->inst[registers[PC]].args[1]);
				continue;
			case MOV:
				mov(registers, ram, pmem->inst[registers[PC]].args[0], pmem->inst[PC].args[1], pmem->inst[PC].args[2]);
				break;
			default:
				return 0;
		}

		if (!inc_PC(registers)) {
			return 0;
		}
	}
	return 1;
}


BYTE get_stk_sym_addr(BYTE *registers, BYTE stk_sym) {
	return registers[FP] + stk_sym;
}

void store(BYTE *registers, BYTE *ram, BYTE A_type, BYTE A, BYTE B) {
	switch (A_type) {
		case REG:
			store_reg(registers, A, B);
			return;
		case STK:
			store_stk(registers, ram, registers[FP] - A, B);
			return;
		case PTR:
			store_stk(registers, ram, access_stk_sym(registers, ram, A), B);
			return;
		default:
			return;

	}
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

BYTE get_data(BYTE *registers, BYTE *ram, BYTE type, BYTE A) {
	switch (type) {
		case PTR:
			return deref_ptr(registers, ram, A);
		case STK:
			return access_stk_sym(registers, ram, A);
		case REG:
			return registers[A];
		default:
			return A;
	}
}

void mov(BYTE *registers, BYTE *ram, BYTE A_type, BYTE A, BYTE B) {
	store(registers, ram, A_type, A, B);
}
void call(struct PMEM *pmem, BYTE *registers, BYTE *ram, BYTE label) {
	push(registers, ram, registers[FP]);
	push(registers, ram, registers[PC]);
	for (registers[GEN] = 0; registers[GEN] < pmem->num_functions; registers[GEN] += 1) {
		if (label == pmem->functions[registers[GEN]].label) {
			registers[PC] = pmem->functions[registers[GEN]].start;
		}
	}
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
void print(BYTE *registers, BYTE *ram, BYTE type, BYTE A) {
	printf("%d\n", get_data(registers, ram, type, A));
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


int inc_PC(BYTE *registers) {
	if (registers[PC] == MEM_SIZE - 1) {
		return 0;
	}
	registers[PC] += 1;
	return 1;
}

void set_PC(BYTE *ptr_reg, BYTE addr) {
	ptr_reg[PC] = addr;
}

BYTE pop(BYTE *registers, BYTE *ram) {
	if (registers[SP] < MEM_SIZE - 1 ) {
		registers[SP] += 1;
		return ram[registers[SP] - 1];
	} 
	return ram[registers[SP]];
}


void push(BYTE *registers, BYTE *ram, BYTE val) {
	if (registers[SP] > 0) {
		registers[SP] -= 1;
		ram[registers[SP] + 1] = val;
	} 
}
