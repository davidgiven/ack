TMP=tmp$$
cat $* >$TMP
ed - $TMP <<'--EOI--'
g/^%/d
g/^$/d
g/^	/.-1,.j
1,$s/[ 	]*|[ 	]*/|/g
g/NOTIMPLEMENTED/d
1,$s/\([^|]*\)|\([^|]*\)|\(.*\)$/\
NAME \1\
HEAD \1\
PLST \2\
DECL \2\
BODY \3/
$a
END
.
g/^NAME/m$
g/^PLST/s/[ 	][ 	]*\([^:]*\):\([^ 	]*\)/,\2/g
g/^PLST,/s//PLST /
g/^PLST /s/^PLST \(.*\)$/(\1)/
g/^HEAD/.,.+1j
g/^HEAD /s/^HEAD \([^(]*\)\(.*\)$/cat >\1.c <<'--EOF--'\
#include "em_private.h"\
\
\1\2/
g/^DECL/s/[ 	][ 	]*\([^:]*\):\([^ 	]*\)/\
	\1 \2;/g
g/^DECL/d
g/^BODY/s/^BODY \(.*\)$/{\
	\1;\
}\
--EOF--/
1,/^END/-1p
1,/^END/d
g:^NAME:s:^NAME \(.*\)$:cc -c -O -I$1 -I$EMHOME/modules/h -I$EMHOME/h \1.c:
1i
cat >make.sh <<'--EOF--'
: script for making lib
rm -f C_*.o
.
$a
rm -f libem$1.a
cc -c -O -I$1 -I$EMHOME/modules/h -I$EMHOME/h $1/em.c
mv em.o em$1.o
ar rc libem$1.a C_*.o em$1.o insert.o io.o failed.o internerr.o getid.o
rm -f C_*.o
--EOF--
.
1,$p
--EOI--
rm -f $TMP
