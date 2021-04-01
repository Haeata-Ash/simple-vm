#include <stdio.h>
#include <stdlib.h>

/* TYPES */
#define VAL 0x00
#define REG 0x01
#define STK 0x02
#define PTR 0x03

/* TYPE LENGTHS */
#define LEN_TYPE 2
#define LEN_OPCODE 3
#define LEN_VALUE 8
#define LEN_REG 3
#define LEN_STK 5
#define LEN_PRT 5

/* OPCODES */
#define MOV 0x00
#define CAL 0x01
#define RET 0x02
#define REF 0x03
#define ADD 0x04
#define PRINT 0x05
#define NOT 0x06
#define EQU 0x07

/* NUMBER OF OPERATION ARGUMENTS */
#define N_MOV 2
#define N_CAL 1
#define N_RET 0
#define N_REF 2
#define N_ADD 2
#define N_PRINT 1
#define N_NOT 1
#define N_EQU 1

unsigned char get_bit_range(unsigned char byte, int start_index, int end);
unsigned char get_section(unsigned char *buf, int index, int bit_cursor, int range);

struct Instruction {
	unsigned char opcode;
	unsigned char type_1;
	unsigned char arg1;
	unsigned char type_2;
	unsigned char arg2;
};

struct function {
	unsigned char label;
	struct Instruction **inst;
};


int main(int argc, char **argv) {
	char *file = argv[1];
	
	FILE *fp = fopen(file, "rb");
	unsigned char buf[10];
	int len = fread(&buf[0], 1, 10, fp);

	for (int i = 0; i < 10; i++) {
		if (i > 0) printf(" ");
		printf("%02x", buf[i]);
	}
	printf("\n");
	printf("\n");
	
	for (int x = len; len >= 0; len--) {
		int num_inst = 0;

	}

	fclose(fp);
	return 1;
}


unsigned char get_section(unsigned char *buf, int index, int bit_cursor, int range) {
	unsigned char current_byte = buf[index];
	unsigned char ret = 0;
	if (bit_cursor + range < 8) {
		ret = get_bit_range(current_byte, bit_cursor, range);
	} else {
		int r = range + bit_cursor - 8;
		ret = get_bit_range(current_byte, bit_cursor, range - r);
		unsigned char next_byte = buf[index - 1];
		next_byte = get_bit_range(next_byte, 0, r) << r;
		ret = ret + next_byte;
	}
	return ret;
}


unsigned char get_bit_range(unsigned char byte, int start_index, int end) {
	int range = end - start_index;
	byte = byte >> (8 - start_index - range);
	byte = byte << (8 - start_index - range);
	return byte;
}

