list=
target=$1
shift
for i
do
	echo "+$cc $cflags $i"
	if $cc $cflags $i
	then
		list="$list `basename $i .c`.o"
	else
		exit 1
	fi
done
echo "+$cc $cflags -I../read_em C_out.c"
if $cc $cflags -I../read_em C_out.c ; then : ; else exit 1 ; fi
echo "+$ar rc libem$target.$libsuf $list"
if $ar rc libem$target.$libsuf $list
then
	:
else
	exit 1
fi
