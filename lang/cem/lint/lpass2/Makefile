# (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
# See the copyright notice in the ACK home directory, in the file "Copyright".
#
# $Header$

#	M A K E F I L E   F O R   L P A S S 2

# Machine and environ dependent definitions
EMHOME =	../../..
LPASS1 =	../lpass1#			???

# Libraries and EM interface definitions
SYSLIB =	$(EMHOME)/modules/lib/libsystem.a
STRLIB =	$(EMHOME)/modules/lib/libstring.a
PRTLIB =	$(EMHOME)/modules/lib/libprint.a
INPLIB =	$(EMHOME)/modules/lib/libinput.a
ALLOCLIB =	$(EMHOME)/modules/lib/liballoc.a
MALLOC =	$(EMHOME)/modules/lib/malloc.o
LLIBS =		$(INPLIB) $(PRTLIB) $(STRLIB) $(ALLOCLIB) $(MALLOC) $(SYSLIB)

CFLAGS =	-I$(EMHOME)/modules/h -I$(EMHOME)/modules/pkg -I/usr/include

.SUFFIXES: .str .h
.str.h:
	$(LPASS1)/make.allocd <$*.str >$*.h

SRC =	lpass2.c checkargs.c read.c report.c class.c l_print3ack.c
OBJ =	lpass2.o checkargs.o read.o report.o class.o l_print3ack.o

test:	lpass2
	lpass2 -xh <proto.tst
#	make lint.test


lpass2:	$(OBJ) Makefile next.o
	$(CC) $(COPTIONS) $(LDFLAGS) $(OBJ) next.o $(LLIBS) -o lpass2
	size lpass2

lint.test:
	./lint -h $(CFLAGS) $(SRC) next.c #???

next.c:	$(LPASS1)/make.next inpdef.str
	$(LPASS1)/make.next inpdef.str > next.c

tags:	$(SRC)
	ctags $(SRC)

clean:
	rm -f a.out core next.c inpdef.h $(OBJ) next.o

#----------------------------------------------------------------
checkargs.o: inpdef.h private.h
class.o: ../lpass1/l_class.h class.h
lpass2.o: ../lpass1/l_class.h ../lpass1/l_lint.h class.h inpdef.h private.h
read.o: ../lpass1/l_class.h class.h inpdef.h private.h
report.o: ../lpass1/l_class.h class.h inpdef.h private.h
