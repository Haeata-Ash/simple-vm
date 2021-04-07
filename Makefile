CC=gcc
CFLAGS=-fsanitize=address -Wvla -Wall -Werror -g -std=gnu11 -lasan

# fill in all your make rules

vm_x2017: vm.c
	#$(CC) $(CFLAGS) $^ -o $@
	echo vm.c

objdump_x2017: parser.c
	$(CC) $(CFLAGS) $^ -o $@

tests:
	echo "tests"

run_tests:
	echo "run_tests"

clean:
	rm vm_x2017 objdump_x2017
