#include <stdlib.h>
#include "vm.h"
#include "parser.h"

#define ARG_LEN 2
#define ARG_LEN_ERR_MSG "Please provide file as command line argument\n"

int main(int argc, char **argv) {
	//initialise pmem
	struct PMEM pmem;
	pmem.num_inst = 0;
	pmem.num_functions = 0;
	BYTE buf[BUF_SIZE];

	// check they gave a cml arg
	if (argc != ARG_LEN) {
		fprintf(stderr, ARG_LEN_ERR_MSG);
		return EXIT_FAILURE;
	}


	// read file into buffer
	char *file = argv[1];
	FILE *fp = fopen(file, "rb");
	int bytes_read = fread(&buf[0], 1, BUF_SIZE, fp);

	// start reading buffer from last byte
	int index = bytes_read - 1;
	int bit_cursor = 0;
	
	// read in each function to pmem
	pmem.num_functions = 0;
	while (index > 0) {
		pmem.functions[pmem.num_functions] = read_function(&buf[0], &index, &bit_cursor, &pmem);
		pmem.num_functions += 1;
	}
	
	//output pmem
	for (int i = pmem.num_functions - 1; i >= 0; i--) {
		output_function(pmem.functions[i], &pmem);
	}

}
