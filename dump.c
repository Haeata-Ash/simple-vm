#include <stdlib.h>
#include "vm.h"
#include "parser.h"

int main(int argc, char **argv) {
	//initialise pmem
	struct PMEM pmem;
	pmem.num_inst = 0;
	pmem.num_functions = 0;
	BYTE buf[BUF_SIZE];

	// check they gave a cml arg
	if (argc != 1) {
		printf("Please give executable as cml argument.\n");
		return EXIT_SUCCESS;
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
