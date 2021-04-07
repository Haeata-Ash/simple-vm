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

int main(int argc, char **argv) {
	struct PMEM pmem;
	pmem.num_inst = 0;
	BYTE buf[BUF_SIZE];


	char *file = argv[1];
	FILE *fp = fopen(file, "rb");
	int bytes_read = fread(&buf[0], 1, BUF_SIZE, fp);
	int index = bytes_read - 1;
	int bit_cursor = 0;
	
	pmem.num_functions = 0;
	while (index > 0) {
		pmem.functions[pmem.num_functions] = read_function(&buf[0], &index, &bit_cursor, &pmem);
		pmem.num_functions += 1;
	}

	for (int i = pmem.num_functions - 1; i >= 0; i--) {
		output_function(pmem.functions[i], &pmem);
	}

}

BYTE parse(FILE *fp, struct PMEM *pmem) {
	BYTE buf[BUF_SIZE];
	int bytes_read = fread(&buf[0], 1, BUF_SIZE, fp);
	int index = bytes_read - 1;
	int bit_cursor = 0;
	
	BYTE count = 0x00;
	while (index > 0) {
		pmem->functions[count] = read_function(&buf[0], &index, &bit_cursor, pmem);
		count += 0x01;
	}

	return count;
}

struct Function read_function(BYTE *buf, int *index, int *bit_cursor, struct PMEM *pmem) {
	struct Function f;
	f.num_inst = get_section(buf, index, bit_cursor, LEN_INST);
	f.start = pmem->num_inst;
	for (int i = f.num_inst - 1; i >= 0; i--) {
		pmem->inst[pmem->num_inst + i] = read_instruction(buf, index, bit_cursor);
	}
	pmem->num_inst += f.num_inst;
	f.label = get_section(buf, index, bit_cursor, LEN_LABEL);
	return f;
}

struct Instruction read_instruction(BYTE *buf, int *index, int *bit_cursor) {
	struct Instruction inst;
	inst.opcode = get_section(buf, index, bit_cursor, LEN_OPCODE);
	inst.num_args = get_num_args(inst.opcode); 
	for (int i = 0; i < inst.num_args * 2; i+=2) {
		inst.args[i] = get_section(buf, index, bit_cursor, LEN_TYPE);
		int arg_len = get_arg_len(inst.args[i]);
		inst.args[i + 1] = get_section(buf, index, bit_cursor, arg_len);
	}

	return inst;
}

unsigned char get_section(BYTE *buf, int *index, int *bit_cursor, int num_bits) {
	BYTE current_byte = buf[*index];
	BYTE ret = 0;
	if (*bit_cursor + num_bits < 8) {
		ret = get_bit_range(current_byte, *bit_cursor, num_bits);
	} else {
		int r = (num_bits + *bit_cursor) % 8;
		ret = get_bit_range(current_byte, *bit_cursor, num_bits - r);
		BYTE next_byte = buf[*index - 1];
		next_byte = get_bit_range(next_byte, 0, r) << (num_bits - r);
		ret = ret + next_byte;
		*index = *index - 1;
	}

	*bit_cursor = (*bit_cursor + num_bits) % 8;
	return ret;
}

unsigned char get_bit_range(BYTE byte, int start_index, int range) {
	byte = byte >> start_index;
	byte = byte & mask(range);
	return byte;
}

unsigned char mask(int num_bits) {
	BYTE full = 0xFF;
	return full >> (8 - num_bits);
}

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
			printf("get_arg len");
			return 0x00;
	}
}

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

void output_arg(BYTE arg, BYTE type) {
	if (type == VAL || type == REG) {
		printf("%d", arg);
	} else if (type == STK || type == PTR) {
		if (arg > 26) {
			printf("%c", arg + 97);
		} else {
			printf("%c", arg+ 65);
		}
	} else {
		printf("error");
	}
}



void output_function(struct Function f, struct PMEM *pmem) {
	printf("FUNC LABEL %d\n", f.label);
	struct Instruction *instructions = pmem->inst;
	struct Instruction inst;
	for (int i = 0; i < f.num_inst; i++) {
		printf("    ");
		inst = instructions[f.start + i];
		if (inst.num_args == 0) {
			output_opcode(inst.opcode);
		} else if (inst.num_args == 1) {
			output_opcode(inst.opcode);
			printf(" ");
			output_type(inst.args[0]);
			printf(" ");
			output_arg(inst.args[1], inst.args[0]);
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



