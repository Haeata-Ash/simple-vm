#!/bin/bash

# Trigger all your test cases with this script
#!/bin/bash

echo "#######################################"
echo "###    Running Operation Tests!     ###"
echo "#######################################"
count=0
for test in tests/basic_operations/*.in; do
	name=$(basename $test .in)
	expected=tests/basic_operations/$name.out

	./vm_x2017 $test | diff - $expected  && printf "[passed] %s\n" $name || printf "\n\n[failed] %s\n" $name
	count=$((count+1))

done

echo
echo "Finished running $count tests!"

echo "#######################################"
echo "###    Running Negative Case Tests!     ###"
echo "#######################################"
count=0
for test in tests/negative_cases/*.in; do
	name=$(basename $test .in)
	expected=tests/negative_cases/$name.out
	./vm_x2017 $test 2> error
	diff error $expected  && printf "[passed] %s\n" $name || printf "\n\n[failed] %s\n" $name
	rm error
	count=$((count+1))

done

echo
echo "Finished running $count tests!"


echo "##################################"
echo "###    Running Edge Tests!     ###"
echo "##################################"
DIR=tests/edge
./vm_x2017 $DIR/overflow_many_func.in 2> err
diff err $DIR/overflow_many_func.out && printf "[passed] %s\n" overflow_many_func || printf "\n\n[failed] %s\n" overflow_many_func
rm err

./vm_x2017 $DIR/out_of_bounds_mem_access.in 2> err
diff err $DIR/out_of_bounds_mem_access.out && printf "[passed] %s\n" out_of_bounds_mem_access || printf "\n\n[failed] %s\n" out_of_bounds_mem_access
rm err

./vm_x2017 2> err
diff err $DIR/no_binary.out && printf "[passed] %s\n" no_binary|| printf "\n\n[failed] %s\n" no_binary
rm err

./vm_x2017 $DIR/bad_move.in 2> err
diff err $DIR/bad_move.out && printf "[passed] %s\n" bad_move || printf "\n\n[failed] %s\n" bad_move
rm err

echo
echo "Finished running 4 tests!"
