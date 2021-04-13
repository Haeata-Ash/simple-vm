#ifndef VM_H_ 
#define VM_H_

/* TYPES */
enum types {
	VAL,
	REG,
	STK,
	PTR,
};

/*LENGTHS IN BITS */
enum {

	LEN_TYPE = 2,
	LEN_OPCODE = 3,
	LEN_VALUE = 8,
	LEN_REG = 3,
	LEN_STK = 5,
	LEN_PRT = 5,
	LEN_INST = 5,
	LEN_LABEL = 3,
};

/* OPCODES VALUE*/
enum opcodes {
	MOV,
	CAL,
	RET,
	REF,
	ADD,
	PRINT,
	NOT,
	EQU,
};

/* NUMBER OF OPERATION ARGUMENTS */
enum op_arg_len {
	N_MOV = 2,
	N_CAL = 1,
	N_RET = 0,
	N_REF = 2,
	N_ADD = 2,
	N_PRINT = 1,
	N_NOT = 1,
	N_EQU = 1,
};

// buffer size based on encoding
#define BUF_SIZE 776

// max instructions that can be encoded in 5 bits
#define MAX_INST 32

// max functions that can be encoded in 3 bits
#define MAX_FUNC 8

// ram size 2^8 byte addressable
#define MEM_SIZE 256

// number of registers
#define NUM_REG 8

// stack pointer register index
#define SP 6 

// program counter register index
#define PC 7 

// frame pointer register index
#define FP 5 

 // status register index
#define STATUS 4

// create BYTE for readability
#define BYTE unsigned char


// status codes that the status register will be set to
enum status_codes {
	NORMAL,
	DONE,
	STK_EMPTY,
	STK_OVERFLOW,
	INVALID_JUMP,
	CALL_0,
	BAD_INSTRUCTION,
	INVALID_REGISTER,
	BAD_ADDR,
	NO_ENTRY_POINT,
};

struct Instruction {
	int opcode;
	int num_args;
	// stored type then arg
	BYTE args[4];
};

struct Function {
	int label;
	int num_inst;
	int start;
	BYTE num_symbols;
	BYTE symbols[32];
};

struct PMEM {
	struct Instruction inst[MAX_INST * MAX_FUNC];
	struct Function functions[MAX_FUNC];
	int num_functions;
	int num_inst;
};

void init_registers(BYTE *registers);

void store_reg(BYTE *registers, BYTE reg, BYTE val);

void store_stk(BYTE *ram, BYTE addr, BYTE val);

void dec_SP(BYTE *registers);

void inc_SP(BYTE *registers);

void store_stk_symbol(BYTE *registers, BYTE *ram, BYTE offset, BYTE val);

BYTE pop(BYTE *registers, BYTE *ram);

void push(BYTE *registers, BYTE *ram, BYTE val);

BYTE access_stk_sym(BYTE *registers, BYTE *ram, BYTE stk_sym);

BYTE get_data(BYTE *registers, BYTE *ram, BYTE type, BYTE A);

void set_error(BYTE *registers, BYTE error_code);

void equ(BYTE reg_addr, BYTE *ptr_reg);

void print(BYTE *registers, BYTE *ram, BYTE type, BYTE A);

void not(BYTE *registers, BYTE reg_addr);

void add(BYTE *registers, BYTE *ram, BYTE A, BYTE B);

void ref(BYTE *registers, BYTE *ram, BYTE A_type, BYTE A, BYTE B_type, BYTE B);

void ret(BYTE *registers, BYTE *stk);

void inc_PC(BYTE *registers);

void call(struct PMEM *pmem, BYTE *registers, BYTE *ram, BYTE label);

void mov(BYTE *registers, BYTE *ram, BYTE A_type, BYTE A, BYTE B_type, BYTE B);

int run(struct PMEM *pmem, BYTE *ram, BYTE *registers);

void error_msg(BYTE code);
#endif
