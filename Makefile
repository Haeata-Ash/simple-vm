CC=gcc
CFLAGS=-fsanitize=address -Wvla -Wall -Werror -g -std=gnu11 -lasan

# fill in all your make rules

vm_x2017: vm.c parser.c
	$(CC) $(CFLAGS) $^ -o $@

objdump_x2017: dump.c parser.c
	$(CC) $(CFLAGS) $^ -o $@

tests:
	echo "tests"

run_tests: vm_x2017
	bash test.sh

clean:
	rm vm_x2017 objdump_x2017
