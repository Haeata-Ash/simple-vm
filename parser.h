#ifndef PARSER_H_
#define PARSER_H_

unsigned char get_bit_range(unsigned char byte, int start_index, int range);

unsigned char get_section(unsigned char *buf, int *index, int *bit_cursor, int range);

unsigned char mask(int num_bits);

struct Instruction read_instruction(unsigned char *buf, int *index, int *bit_cursor);

int get_num_args(unsigned char op);

int get_arg_len(unsigned char type);

struct Function read_function(unsigned char *buf, int *index, int *bit_cursor);

void output_function(struct Function f);

#endif
