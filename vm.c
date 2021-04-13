#include <stdio.h>
#include <stdlib.h>
#include "vm.h"
#include "parser.h"



int main(int argc, char **argv) {
	// create program memory and give initial values
	struct PMEM pmem;
	pmem.num_inst = 0;
	pmem.num_functions = 0;

	
	// ram and registers
	BYTE ram[MEM_SIZE];
	BYTE registers[NUM_REG];

	// give registers initial values
	init_registers(&registers[0]);

	//read in program to pmem
	if (argc != 2) {
		fprintf(stderr, "Please provide file as command line argument\n");
		return EXIT_FAILURE;
	}
	char *file = argv[1];
	FILE *fp = fopen(file, "rb");
	parse(fp, &pmem);

	// begin executing instructions
	return run(&pmem, &ram[0], &registers[0]);
}

// give register initial values
void init_registers(BYTE *registers) {
	for (int i = 0; i < 8; i++) {
		if (i == SP || i == FP) {
			registers[i] = 0xFF;
		} else if (i == STATUS) {
			registers[i] = NO_ENTRY_POINT;
		}
		else {
			registers[i] = 0;
		}
	}
}

void print_vm_state(BYTE *registers, BYTE *ram) {
	printf("FP: %d  || ", registers[FP]);
	printf("SP: %d  || ", registers[SP]); 
	printf("PC: %d\n\n", registers[PC]);

	for (int i = 0; i < 4; i++) {
		printf("REGISTER %d: %d\n", i, registers[i]);
	}
	for (int i = 255; i > registers[SP]; i--) {
		printf("Stack Addr %d: %d\n", i, ram[i]);
	}
	printf("#######################################\n\n");
}


int run(struct PMEM *pmem, BYTE *ram, BYTE *registers) {
	int num_inst = pmem->num_inst;
	struct Instruction i;

	//find entry point
	
	for (int j = 0; j < pmem->num_functions; j++) {
		if (pmem->functions[j].label == 0) {
			registers[PC] = pmem->functions[j].start;
			set_error(registers, NORMAL);
		}
	}

	if (registers[STATUS] == NO_ENTRY_POINT) {
		error_msg(NO_ENTRY_POINT);
		return EXIT_FAILURE;
	}

	// execute instruction pointed to by program counter until non left
	while (registers[PC] < num_inst) {

		// current instruction 
		i = pmem->inst[registers[PC]];

		// find the operation required
		switch(i.opcode) {
			case EQU:
				equ(i.args[1], registers);
				break;
			case NOT:
				not(registers, i.args[1]);
				break;
			case PRINT:
				print(registers, ram, i.args[0], i.args[1]);
				break;
			case ADD:
				add(registers, ram, i.args[1], i.args[3]);
				break;
			case REF:
				ref(registers,
				    ram, 
				    i.args[0], 
				    i.args[1], 
				    i.args[2],
				    i.args[3]
				);
				break;

			case RET:
				ret(registers, ram);
				break;
			case CAL:
				call(pmem, registers, ram, i.args[1]);
				break;
			case MOV:
				mov(registers,
				    ram, 
				    i.args[0], 
				    i.args[1], 
				    i.args[2], 
				    i.args[3]
				);
				break;
			default:
				set_error(registers, BAD_INSTRUCTION);
		}

		switch (registers[STATUS]) {
			case CALL_0:
				break;
			case STK_OVERFLOW:
				error_msg(STK_OVERFLOW);
				return EXIT_FAILURE;
			case STK_EMPTY:
				error_msg(STK_EMPTY);
				return EXIT_FAILURE;
			case BAD_INSTRUCTION:
				error_msg(BAD_INSTRUCTION);
				return EXIT_FAILURE;
			case BAD_ADDR:
				error_msg(BAD_ADDR);
				return EXIT_FAILURE;
			case UNDEFINED_SYMBOL:
				error_msg(UNDEFINED_SYMBOL);
				return EXIT_FAILURE;
			case DONE:
				return EXIT_SUCCESS;
			default:
				inc_PC(registers);
				break;
		}
		registers[STATUS] = NORMAL;
	}
	return 1;
}

// stores a value B in the appropriate memory type
void store(BYTE *registers, BYTE *ram, BYTE A_type, BYTE A, BYTE B) {
	switch (A_type) {
		case REG:
			//storing in a register
			store_reg(registers, A, B);
			return;
		case STK:
			//storing in a symbol
			store_stk_symbol(registers, ram, A, B);
			return;
		case PTR:
			// storing in a pointer
			store_stk(ram, access_stk_sym(registers, ram, A), B);
			return;
		default:
			set_error(registers, BAD_INSTRUCTION);
			return;
	}
}

// stores a value in a register
void store_reg(BYTE *registers, BYTE reg, BYTE val) {
	// check if its a valid register
	if (reg >= 8) {
		set_error(registers, INVALID_REGISTER);
		return;
	}
	registers[reg] = val;
}

// stores a value at addr in RAM
void store_stk(BYTE *ram, BYTE addr, BYTE val) {
	// always valid addr since a BYTE 0-255
	ram[addr] = val;
}

// stores a value in side of a symbol
void store_stk_symbol(BYTE *registers, BYTE *ram, BYTE offset, BYTE val) {
	// check if the symbol is already on the stack by comparing its offset
	// to the current stack frame size
	
	if (offset >= registers[FP] - registers[SP]) {
		//push new symbol onto stack
		push(registers, ram, val);

	} else {
		// replace an existing symbol
		BYTE addr = registers[FP] - offset;
		store_stk(ram, addr, val);
	}
}

// get the address of a stack symbol
BYTE get_stk_sym_addr(BYTE *registers, BYTE stk_sym) {
	//take start of stack frame and offset relative stk symbol
	return registers[FP] - stk_sym;
}

// get the contents of a stack symbol
BYTE access_stk_sym(BYTE *registers, BYTE *ram, BYTE offset) {

	// get stack symbol location by offsetting from frame pointer
	BYTE addr = registers[FP] - offset;
	if (addr <= registers[SP]) {
		set_error(registers, UNDEFINED_SYMBOL);
		return 0;
	}
	return ram[addr];
}


BYTE deref_ptr(BYTE *registers, BYTE *ram, BYTE stk_sym) {
	// get stk symbol value then use it as an address in ram
	BYTE addr = access_stk_sym(registers, ram, stk_sym);
	
	// check if the addr is valid
	if (addr <= SP) {
		set_error(registers, BAD_ADDR);
		return EXIT_FAILURE;
	}
	return ram[addr];
}

// retrieves contents of a point in memory A of type A
BYTE get_data(BYTE *registers, BYTE *ram, BYTE type, BYTE A) {
	switch (type) {
		case PTR:
			return deref_ptr(registers, ram, A);
		case STK:
			return access_stk_sym(registers, ram, A);
		case REG:
			return registers[A];
		case VAL:
			return A;
		default:
			set_error(registers, BAD_INSTRUCTION);
			return A;
	}
}

// sets the status register to a given error code
void set_error(BYTE *registers, BYTE error_code) {
	registers[STATUS] = error_code;
}

// prints an error msg based on the code 
void error_msg(BYTE code) {
	switch (code) {
		case STK_OVERFLOW:
			fprintf(stderr, "Stack overflow\n");
			break;
		case INVALID_JUMP:
			fprintf(stderr, "Jumped to invalid program counter value\n");
			break;
		case BAD_INSTRUCTION:
			fprintf(stderr, "Bad instructions\n");
			break;
		case INVALID_REGISTER:
			fprintf(stderr, "Invalid register\n");
			break;
		case BAD_ADDR:
			fprintf(stderr, "Accessed invalid memory address\n");
			break;
		case NO_ENTRY_POINT:
			fprintf(stderr, "No entry point in program\n");
			break;
		case STK_EMPTY:
			fprintf(stderr, "Underflow?\n");
			break;
		case UNDEFINED_SYMBOL:
			fprintf(stderr, "Accessed undefined symbol\n");
			break;
		default:
			fprintf(stdin, "Unspecified error occured\n");

	}

}

// moves a value B of type B_type to the point A in memory of type A_type
void mov(BYTE *registers, BYTE *ram, BYTE A_type, BYTE A, BYTE B_type, BYTE B) {
	store(registers, ram, A_type, A, get_data(registers, ram, B_type, B));
}

// calls a given function by label
void call(struct PMEM *pmem, BYTE *registers, BYTE *ram, BYTE label) {

	// push the existing frame pointer and program counter to the stack
	push(registers, ram, registers[FP]);
	push(registers, ram, registers[PC]);

	// set the start of the new frame to the next position in the stack
	registers[FP] = registers[SP];

	// find the function label
	for (int i = 0; i < pmem->num_functions; i += 1) {
		if (label == pmem->functions[i].label) {

			// set the program counter to the start the instruction
			if (pmem->functions[i].start == 0) {
				// special case where the first instruction is 
				// at position 0 and the PC should not be
				// incremented
				registers[STATUS] = CALL_0;
				registers[PC] = pmem->functions[i].start;

			} else {
				registers[PC] = pmem->functions[i].start- 1;
			}
		} 
	}
}

// returns from a function
void ret(BYTE *registers, BYTE *stk) {
	// find current frames size
	BYTE frame_size = registers[FP] - registers[SP];

	// remove all symbols
	while (frame_size > 0) {
		pop(registers, stk);
		frame_size -= 1;
	}
	
	// set PC to next instruction and FP to start of the frame that called
	registers[PC] = pop(registers, stk);
	registers[FP] = pop(registers, stk);

	// if the stack empty flag is given after the previous pops,
	// we have returned from function 0
	if (registers[STATUS] == STK_EMPTY) {
		registers[STATUS] = DONE;
	}
}

// stores the address of B in A
void ref(BYTE *registers, BYTE *ram, BYTE A_type, BYTE A, BYTE B_type, BYTE B) {

	// dereferencing the pointer and finding its address cancel out
	if (B_type == PTR) {
		store(registers, ram, A_type, A, access_stk_sym(registers, ram, B));

	// get the address of the stk symbol B
	} else if (B_type == STK){
		store(registers, ram, A_type, A, get_stk_sym_addr(registers, B));
	} else {
		set_error(registers, BAD_INSTRUCTION);
	}
}

// adds to registers together and stores in register A
void add(BYTE *registers, BYTE *ram, BYTE A, BYTE B) {
	store_reg(registers, A, registers[A] + registers[B]);
 }

// prints the contents of A where A can be any type
void print(BYTE *registers, BYTE *ram, BYTE type, BYTE A) {
	printf("%d\n", get_data(registers, ram, type, A));
}

// bitwise not operation on a register
void not(BYTE *registers, BYTE reg_addr) {
	store_reg(registers, reg_addr, ~(registers[reg_addr]));
}

// checks if a register is equal to 0
void equ(BYTE reg_addr, BYTE *ptr_reg) {
	if (ptr_reg[reg_addr] == 0) {
		ptr_reg[reg_addr] = 0x01;
	} else {
		ptr_reg[reg_addr] = 0x00;
	}
}

// increments the Program Counter
void inc_PC(BYTE *registers) {

	if (registers[PC] == 255) {
		registers[STATUS] = DONE;
	} else {
		registers[PC] += 1;
	}
}

// increments the stack pointer (stk empty when stack pointer set to 255)
void inc_SP(BYTE *registers) {
	// STK is empty
	if (registers[SP] == MEM_SIZE - 1) {
		set_error(registers, STK_EMPTY);

	} else {
		registers[SP] += 1;
	}
}

// decrement stack (grows stack)
void dec_SP(BYTE *registers) {
	// stack is already full
	if (registers[SP] == 0) {
		set_error(registers, STK_OVERFLOW);
	}

	else {
		registers[SP] -= 1;
	}
}

// set the program counter to jump program
void set_PC(BYTE num_inst, BYTE *registers, BYTE new_pc) {
	// check that instruction exists
	if (registers[PC] >= num_inst) {
		set_error(registers, INVALID_JUMP);
		return;
	}
	registers[PC] = new_pc;
}

// remove element from stack
BYTE pop(BYTE *registers, BYTE *ram) {
	inc_SP(registers);
	return ram[registers[SP]];
}

// add element to stack
void push(BYTE *registers, BYTE *ram, BYTE val) {
	ram[registers[SP]] = val;
	dec_SP(registers);
}
