DIR=$1

for f in $DIR/*.asm; do
	base=$(basename -s .asm $f)
	echo $DIR/$base.in
	python3 ~/code-submission/compiler.py $f $DIR/$base.in
done
