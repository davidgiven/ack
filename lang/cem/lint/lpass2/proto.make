# (c) copyright 1987 by the Vrije Universiteit, Amsterdam, The Netherlands.
# See the copyright notice in the ACK home directory, in the file "Copyright".
#
# $Header$

#	M A K E F I L E   F O R   L P A S S 2

#PARAMS		do not remove this line!

SRC_DIR =	$(SRC_HOME)/lang/cem/lint/lpass2
LPASS1 =	$(SRC_HOME)/lang/cem/cemcom

# Libraries and EM interface definitions
SYSLIB =	$(TARGET_HOME)/modules/lib/libsystem.$(LIBSUF)
STRLIB =	$(TARGET_HOME)/modules/lib/libstring.$(LIBSUF)
PRTLIB =	$(TARGET_HOME)/modules/lib/libprint.$(LIBSUF)
INPLIB =	$(TARGET_HOME)/modules/lib/libinput.$(LIBSUF)
ALLOCLIB =	$(TARGET_HOME)/modules/lib/liballoc.$(LIBSUF)
MALLOC =	$(TARGET_HOME)/modules/lib/malloc.$(SUF)
LLIBS =		$(INPLIB) $(PRTLIB) $(STRLIB) $(ALLOCLIB) $(MALLOC) $(SYSLIB)

INCLUDES =	-I$(TARGET_HOME)/modules/h -I$(TARGET_HOME)/modules/pkg \
		-I$(SRC_DIR) -I$(LPASS1) -I.
CFLAGS = $(COPTIONS) $(INCLUDES)
LDFLAGS = $(LDOPTIONS)
LINTFLAGS = $(LINTOPIONS) $(INCLUDES)

SRC =	$(SRC_DIR)/lpass2.c $(SRC_DIR)/checkargs.c \
	$(SRC_DIR)/read.c $(SRC_DIR)/report.c \
	$(SRC_DIR)/class.c $(SRC_DIR)/l_print3ack.c
OBJ =	lpass2.$(SUF) checkargs.$(SUF) read.$(SUF) report.$(SUF) class.$(SUF)

all:	lpass2 lint.exec

install:	all
	-mkdir $(TARGET_HOME)/lib.bin/lint
	cp lpass2 $(TARGET_HOME)/lib.bin/lint/lpass2
	cp lint.exec $(TARGET_HOME)/bin/lint
	if [ $(DO_MACHINE_INDEP) = y ] ; \
	then	mk_manpage $(SRC_DIR)/lint.1 $(TARGET_HOME) ; \
	fi

cmp:	all
	-cmp lpass2 $(TARGET_HOME)/lib.bin/lint/lpass2
	-cmp lint.exec $(TARGET_HOME)/bin/lint

pr:
	@pr $(SRC_DIR)/proto.make $(SRC_DIR)/inpdef.str $(SRC_DIR)/class.h \
		$(SRC) lint

lint.exec:	$(SRC_DIR)/lint
	sed -e 's|^EMDIR=.*|EMDIR=$(TARGET_HOME)|' -e 's/#.*//' < $(SRC_DIR)/lint > lint.exec
	chmod +x lint.exec

lpass2:	$(OBJ) next.$(SUF)
	$(CC) $(LDFLAGS) $(OBJ) next.$(SUF) $(LLIBS) -o lpass2

next.c:	$(LPASS1)/make.next $(SRC_DIR)/inpdef.str
	$(LPASS1)/make.next $(SRC_DIR)/inpdef.str > next.c

tags:	$(SRC)
	ctags $(SRC)

clean:
	rm -f a.out core next.c inpdef.h *.$(SUF) lint.exec lpass2

lint:	inpdef.h next.c
	$(LINT) $(LINTFLAGS) $(SRC) next.c

inpdef.h:	$(LPASS1)/make.allocd $(SRC_DIR)/inpdef.str
	$(LPASS1)/make.allocd <$(SRC_DIR)/inpdef.str > inpdef.h

next.$(SUF):	next.c
	$(CC) -c $(CFLAGS) next.c

checkargs.$(SUF): inpdef.h $(SRC_DIR)/private.h $(SRC_DIR)/checkargs.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/checkargs.c

class.$(SUF): $(LPASS1)/l_class.h $(SRC_DIR)/class.h $(SRC_DIR)/class.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/class.c

lpass2.$(SUF): $(LPASS1)/l_class.h $(LPASS1)/l_lint.h $(SRC_DIR)/class.h \
	$(SRC_DIR)/private.h inpdef.h $(SRC_DIR)/lpass2.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/lpass2.c

read.$(SUF): $(LPASS1)/l_class.h $(SRC_DIR)/class.h \
	$(SRC_DIR)/private.h inpdef.h $(SRC_DIR)/read.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/read.c

report.$(SUF): $(LPASS1)/l_class.h $(SRC_DIR)/class.h \
	$(SRC_DIR)/private.h inpdef.h $(SRC_DIR)/report.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/report.c
