#ifndef VM_H_ 
#define VM_H_

/* TYPES */
#define VAL 0x00
#define REG 0x01
#define STK 0x02
#define PTR 0x03

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

/* NUMBER OF OPERATION ARGUMENTS */
#define N_MOV 2
#define N_CAL 1
#define N_RET 0
#define N_REF 2
#define N_ADD 2
#define N_PRINT 1
#define N_NOT 1
#define N_EQU 1


#define BUF_SIZE 776
#define MAX_INST 32
#define MAX_FUNC 8

#define BYTE unsigned char

struct Instruction {
	int opcode;
	int num_args;
	// stored type then arg
	BYTE args[4];
};

struct Function {
	int label;
	int num_inst;
	struct Instruction inst[MAX_INST];
};

#endif
