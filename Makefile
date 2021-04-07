CC=gcc
CFLAGS=-fsanitize=address -Wvla -Wall -Werror -g -std=gnu11 -lasan

# fill in all your make rules

vm_x2017: vm.c
	$(CC) $^ parser.c -o $@

objdump_x2017: dump.c
	$(CC) $^ parser.c -o $@

tests:
	echo "tests"

run_tests:
	echo "run_tests"

clean:
	rm vm_x2017 objdump_x2017
