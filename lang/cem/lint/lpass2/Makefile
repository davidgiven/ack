#	M A K E F I L E   F O R   L P A S S 2

# Machine and environ dependent definitions
EMHOME = /usr/em
LPASS1 = ../lpass1

# Libraries and EM interface definitions
SYSLIB = $(EMHOME)/modules/lib/libsystem.a
STRLIB = $(EMHOME)/modules/lib/libstring.a
PRTLIB = $(EMHOME)/modules/lib/libprint.a
INPLIB = $(EMHOME)/modules/lib/libinput.a
ALLOCLIB = $(EMHOME)/modules/lib/liballoc.a
MALLOC = $(EMHOME)/modules/lib/malloc.o
LLIBS = $(INPLIB) $(PRTLIB) $(STRLIB) $(ALLOCLIB) $(MALLOC) $(SYSLIB)

CFLAGS = -I$(EMHOME)/modules/h -I$(EMHOME)/modules/pkg -I/usr/include

.SUFFIXES: .str .h
.str.h:
	$(LPASS1)/make.allocd <$*.str >$*.h

lpass2:	lpass2.o makefile next.o
	$(CC) $(COPTIONS) $(LDFLAGS) lpass2.o next.o $(LLIBS) -o lpass2
	size lpass2

lint:
	lint $(CFLAGS) lpass2.c next.c

next.c:	inpdef.str
	$(LPASS1)/make.next inpdef.str > next.c

clean:
	rm -f a.out core next.c inpdef.h lpass2.o next.o

#----------------------------------------------------------------
lpass2.o: ../lpass1/errout.h ../lpass1/lint.h ../lpass1/manifest.h inpdef.h
