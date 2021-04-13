#include <stdio.h>
#include <stdlib.h>
#include "vm.h"
#include "parser.h"

/* TYPES STRINGS */
#define S_VAL "VAL"
#define S_REG "REG"
#define S_STK "STK"
#define S_PTR "PTR"

/* OPCODES STRINGS*/
#define S_MOV "MOV"
#define S_CAL "CAL"
#define S_RET "RET"
#define S_REF "REF"
#define S_ADD "ADD"
#define S_PRINT "PRINT"
#define S_NOT "NOT"
#define S_EQU "EQU"

// read executable from file and store as instructions in pmem
void parse(FILE *fp, struct PMEM *pmem) {
	BYTE buf[BUF_SIZE];
	int bytes_read = fread(&buf[0], 1, BUF_SIZE, fp);

	// start reading from last byte to avoid padding bits 
	int index = bytes_read - 1;

	// current bit of a byte being read
	int bit_cursor = 0;

	// read in each function and store in pmem along with instructions
	int num_f = 0;
	while (index > 0) {
		num_f = pmem->num_functions;

		// read function
		pmem->functions[num_f] = read_function(&buf[0], &index, &bit_cursor, pmem);
		pmem->num_functions += 1;
	}
}

// replace symbols with an offset value according to the order they appear
// i.e if Z was first, it would recieve an offset value of 0, while the second
// would get an offset of 1 and so on
BYTE replace_symbol(struct Function *f, BYTE symbol) {

	// iterate through symbols
	for (BYTE i = 0; i < f->num_symbols; i++) {

		// if symbol exists, give it the offset equal to its index 
		if (symbol == f->symbols[i]) {
			return i;
		}
	}

	// if it doesn't already exist, add it to the list of symbols
	// and give it an offset
	f->symbols[f->num_symbols] = symbol;
	f->num_symbols += 1;
	return f->num_symbols - 1;
 }

// read in a function from buffer and update current byte index and bit_cursor
struct Function read_function(BYTE *buf, int *index, int *bit_cursor, struct PMEM *pmem) {
	//intialize new function
	struct Function f;
	f.num_symbols = 0;

	// get the no. of instructions and the functions start pc
	f.num_inst = get_section(buf, index, bit_cursor, LEN_INST);
	f.start = pmem->num_inst;

	//add each instruction to program memory
	for (int i = f.num_inst - 1; i >= 0; i--) {

		//get instruction pc value
		int inst_index = pmem->num_inst + i;

		// read in the instruction
		pmem->inst[inst_index] = read_instruction(&f, buf, index, bit_cursor);
	}

	// replace the function symbols with local offset values 
	for (int i = 0; i < f.num_inst; i++) {

		// get an instruction in function 
		struct Instruction *inst = &pmem->inst[f.start + i];
		
		int len = inst->num_args;
		BYTE *args = inst->args;
		for (int j = 0; j < len; j+=2) {

			// replace args that are symbols with offset
			if (args[j] == PTR || args[j] == STK) {
				args[j + 1] = replace_symbol(&f,args[j + 1]);
			}
		}
	}

	// increment number of instructions
	pmem->num_inst += f.num_inst;

	// add function label
	f.label = get_section(buf, index, bit_cursor, LEN_LABEL);
	return f;
}

// read an instruction and store it in pmem
struct Instruction read_instruction(struct Function *f, BYTE *buf, int *index, int *bit_cursor) {
	// initialise instruction
	struct Instruction inst;

	// read in its opcode and the number of args
	inst.opcode = get_section(buf, index, bit_cursor, LEN_OPCODE);
	inst.num_args = get_num_args(inst.opcode) * 2; 

	// read in its arguments and types
	for (int i = 0; i < inst.num_args; i+=2) {
		//type 
		inst.args[i] = get_section(buf, index, bit_cursor, LEN_TYPE);
		int arg_len = get_arg_len(inst.args[i]);
		
		// argument value
		inst.args[i + 1] = get_section(buf, index, bit_cursor, arg_len);
	}

	return inst;
}

// read in a section of bits of specified length from buffer
BYTE get_section(BYTE *buf, int *index, int *bit_cursor, int num_bits) {
	BYTE current_byte = buf[*index];
	BYTE ret = 0;

	// if all the bits are in the current byte, return specified bits
	if (*bit_cursor + num_bits < 8) {
		ret = get_bit_range(current_byte, *bit_cursor, num_bits);
	
	// not all bits are in the current byte
	} else {
		// remainder bits not in current byte
		int r = (num_bits + *bit_cursor) % 8;

		//get bits in current byte
		ret = get_bit_range(current_byte, *bit_cursor, num_bits - r);

		// if remainder was 0 return (due to index's not playing nice)
		if (r == 0) {
			*bit_cursor = (*bit_cursor + num_bits) % 8;
			*index = *index - 1;
			return ret;
		}

		// r is not 0, read in next byte and get remainder bits
		BYTE next_byte = buf[*index - 1];

		//concat bits
		next_byte = get_bit_range(next_byte, 0, r) << (num_bits - r);
		ret = ret + next_byte;

		// update buffer byte index
		*index = *index - 1;
	}
	
	//update bits cursor and return bits
	*bit_cursor = (*bit_cursor + num_bits) % 8;
	return ret;
}

// isolate bits in a byte
BYTE get_bit_range(BYTE byte, int start_index, int range) {
	//shift them to the start
	byte = byte >> start_index;

	// mask any foollowing bits out
	byte = byte & mask(range);
	return byte;
}

// gives a bit mask for n bits from the front
BYTE mask(int num_bits) {
	BYTE full = 0xFF;
	return full >> (8 - num_bits);
}

// get bit length of an argument type
int get_arg_len(BYTE type) {
	switch (type) {
		case (VAL):
			return LEN_VALUE;
		case (REG):
			return LEN_REG;
		case (STK):
			return LEN_STK;
		case (PTR):
			return LEN_PRT;
		default:
			return 0x00;
	}
}

// get the number of arguments for an operation
int get_num_args(BYTE op) {
	switch (op) {
		case (MOV):
			return N_MOV;
		case (CAL):
			return N_CAL;
		case (RET):
			return N_RET;
		case (REF):
			return N_REF;
		case (ADD):
			return N_ADD;
		case (PRINT):
			return N_PRINT;
		case (NOT):
			return N_NOT;
		case (EQU):
			return N_EQU;
		default:
			printf("num args");
			return 0x00;
	}
}

// print an opcode
void output_opcode(int op) {
	switch (op) {
		case (MOV):
			printf("%s", S_MOV);
			return;
		case (CAL):
			printf("%s", S_CAL);
			return;
		case (RET):
			printf("%s", S_RET);
			return;
		case (REF):
			printf("%s", S_REF);
			return;
		case (ADD):
			printf("%s", S_ADD);
			return;
		case (PRINT):
			printf("%s", S_PRINT);
			return;
		case (NOT):
			printf("%s", S_NOT);
			return;
		case (EQU):
			printf("%s", S_EQU);
			return;
		default:
			printf("error");
			return;
	}
}

// print a type
void output_type(BYTE type) {
	switch (type) {
		case (VAL):
			printf("%s", S_VAL);
			return;
		case (REG):
			printf("%s", S_REG);
			return;
		case (STK):
			printf("%s", S_STK);
			return;
		case (PTR):
			printf("%s", S_PTR);
			return;
		default:
			printf("error");
			return;
	}
}

// print a stk symbol with letter corresponding to the order they appear
void print_stk(int val) {
		if (val > 26) {
			printf("%c", val + 97);
		} else {
			printf("%c", val + 65);
		}

}

// print an argument
void output_arg(BYTE arg, BYTE type) {
	if (type == VAL || type == REG) {
		printf("%d", arg);
	} else {
		print_stk(arg);
	} 
}


// print out whole function and its instructions
void output_function(struct Function f, struct PMEM *pmem) {

	//label
	printf("FUNC LABEL %d\n", f.label);
	
	// inst to print
	struct Instruction *instructions = pmem->inst;
	struct Instruction inst;
	for (int i = 0; i < f.num_inst; i++) {
		printf("    ");
		inst = instructions[f.start + i];

		//instruction has 0 argumennts
		if (inst.num_args == 0) {
			output_opcode(inst.opcode);

		//instruction has 1 argumennts
		} else if (inst.num_args == 2) {
			output_opcode(inst.opcode);
			printf(" ");
			output_type(inst.args[0]);
			printf(" ");
			output_arg(inst.args[1], inst.args[0]);

		//instruction has 2 argumennts
		} else {
			output_opcode(inst.opcode);
			printf(" ");
			output_type(inst.args[0]);
			printf(" ");
			output_arg(inst.args[1], inst.args[0]);
			printf(" ");
			output_type(inst.args[2]);
			printf(" ");
			output_arg(inst.args[3], inst.args[2]);
		}
		printf("\n");
	}
}



