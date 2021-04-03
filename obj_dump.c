#include <stdio.h>
#include <stdlib.h>

/* TYPES */
#define VAL 0x00
#define REG 0x01
#define STK 0x02
#define PTR 0x03

/* TYPES STRINGS */
#define S_VAL "VAL"
#define S_REG "REG"
#define S_STK "STK"
#define S_PTR "PTR"

/*LENGTHS IN BITS */
#define LEN_TYPE 2
#define LEN_OPCODE 3
#define LEN_VALUE 8
#define LEN_REG 3
#define LEN_STK 5
#define LEN_PRT 5
#define LEN_INST 5
#define LEN_LABEL 3

/* OPCODES VALUE*/
#define MOV 0x00
#define CAL 0x01
#define RET 0x02
#define REF 0x03
#define ADD 0x04
#define PRINT 0x05
#define NOT 0x06
#define EQU 0x07

/* OPCODES STRINGS*/
#define S_MOV "MOV"
#define S_CAL "CAL"
#define S_RET "RET"
#define S_REF "REF"
#define S_ADD "ADD"
#define S_PRINT "PRINT"
#define S_NOT "NOT"
#define S_EQU "EQU"

/* NUMBER OF OPERATION ARGUMENTS */
#define N_MOV 2
#define N_CAL 1
#define N_RET 0
#define N_REF 2
#define N_ADD 2
#define N_PRINT 1
#define N_NOT 1
#define N_EQU 1


#define MEM_SIZE 256
#define BUF_SIZE 776
#define MAX_INST 32
#define MAX_FUNC 8

unsigned char get_bit_range(unsigned char byte, int start_index, int range);
unsigned char get_section(unsigned char *buf, int *index, int *bit_cursor, int range);
unsigned char mask(int num_bits);
struct Instruction read_instruction(unsigned char *buf, int *index, int *bit_cursor);
int get_num_args(unsigned char op);
int get_arg_len(unsigned char type);
struct Function read_function(unsigned char *buf, int *index, int *bit_cursor);
void output_function(struct Function f);



struct Instruction {
	int opcode;
	int num_args;
	// stored type then arg
	unsigned char args[4];
};

struct Function {
	int label;
	int num_inst;
	struct Instruction inst[MAX_INST];
};

int main(int argc, char **argv) {
	struct Function functions[MAX_FUNC];
	unsigned char buf[BUF_SIZE];


	char *file = argv[1];
	FILE *fp = fopen(file, "rb");
	int bytes_read = fread(&buf[0], 1, BUF_SIZE, fp);
	int index = bytes_read - 1;
	int bit_cursor = 0;
	
	int count = 0;
	while (index > 0) {
		functions[count] = read_function(&buf[0], &index, &bit_cursor);
		count += 1;
	}

	for (int i = 0; i < count; i++) {
		output_function(functions[i]);
	}

}

struct Function read_function(unsigned char *buf, int *index, int *bit_cursor) {
	struct Function f;
	f.num_inst = get_section(buf, index, bit_cursor, 5);
	for (int i = f.num_inst - 1; i >= 0; i--) {
		f.inst[i] = read_instruction(buf, index, bit_cursor);
	}
	f.label = get_section(buf, index, bit_cursor, LEN_LABEL);
	return f;
}

struct Instruction read_instruction(unsigned char *buf, int *index, int *bit_cursor) {
	struct Instruction inst;
	inst.opcode = get_section(buf, index, bit_cursor, 3);
	inst.num_args = get_num_args(inst.opcode); 
	for (int i = 0; i < inst.num_args * 2; i+=2) {
		inst.args[i] = get_section(buf, index, bit_cursor, LEN_TYPE);
		int arg_len = get_arg_len(inst.args[i]);
		inst.args[i + 1] = get_section(buf, index, bit_cursor, arg_len);
	}

	return inst;
}

unsigned char get_section(unsigned char *buf, int *index, int *bit_cursor, int num_bits) {
	unsigned char current_byte = buf[*index];
	unsigned char ret = 0;
	if (*bit_cursor + num_bits < 8) {
		ret = get_bit_range(current_byte, *bit_cursor, num_bits);
	} else {
		int r = (num_bits + *bit_cursor) % 8;
		ret = get_bit_range(current_byte, *bit_cursor, num_bits - r);
		unsigned char next_byte = buf[*index - 1];
		next_byte = get_bit_range(next_byte, 0, r) << (num_bits - r);
		ret = ret + next_byte;
		*index = *index - 1;
	}

	*bit_cursor = (*bit_cursor + num_bits) % 8;
	return ret;
}

unsigned char get_bit_range(unsigned char byte, int start_index, int range) {
	byte = byte >> start_index;
	byte = byte & mask(range);
	return byte;
}

unsigned char mask(int num_bits) {
	unsigned char full = 0xFF;
	return full >> (8 - num_bits);
}

int get_arg_len(unsigned char type) {
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

int get_num_args(unsigned char op) {
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

void output_type(unsigned char type) {
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

void output_arg(unsigned char arg, unsigned char type) {
	if (type == VAL || type == REG || type == PTR) {
		printf("%d", arg);
	} else if (type == STK) {
		if (arg > 26) {
			printf("%c", arg + 97);
		} else {
			printf("%c", arg+ 65);
		}
	} else {
		printf("error");
	}
}



void output_function(struct Function f) {
	printf("FUNC LABEL %d\n", f.label);
	struct Instruction inst;
	for (int i = 0; i < f.num_inst; i++) {
		printf("    ");
		inst = f.inst[i];
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



