list=
target=$1
shift
for i
do
	echo "+$cc $cflags $i"
	if $cc $cflags $i
	then
		list="$list `basename $i .c`.$suf"
	else
		exit 1
	fi
done
echo "+$ar r libem$target.$libsuf $list"
if $ar r libem$target.$libsuf $list
then
	:
else
	exit 1
fi
