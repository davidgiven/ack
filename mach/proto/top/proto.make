# $Id$

#PARAMS		do not remove this line!

#MACH_DEFINE	do not remove this or the next line!
MACH = 

SRC_DIR = $(SRC_HOME)/mach/$(MACH)/top
LIBS=$(TARGET_HOME)/modules/lib/libstring.$(LIBSUF)
PREFLAGS=-I. -DNDEBUG
PFLAGS=
CFLAGS=$(PREFLAGS) $(PFLAGS) $(COPTIONS)
LDFLAGS=$(PFLAGS) $(LDOPTIONS)
LINTFLAGS=$(PREFLAGS) $(LINTOPTIONS)
CDIR=$(SRC_HOME)/mach/proto/top
CFILES=$(CDIR)/top.c $(CDIR)/queue.c
OFILES=top.$(SUF) queue.$(SUF)

all:	gen.c
	make top

top:	$(OFILES)
	$(CC) $(LDFLAGS) $(OFILES)  $(LIBS) -o top

top.$(SUF):	$(CDIR)/top.c gen.c
	$(CC) -c $(CFLAGS) $(CDIR)/top.c

queue.$(SUF): $(CDIR)/queue.c
	$(CC) -c $(CFLAGS) $(CDIR)/queue.c

install: all
	-mkdir $(TARGET_HOME)/lib.bin/$(MACH)
	cp top $(TARGET_HOME)/lib.bin/$(MACH)/top

cmp:	 all
	-cmp top $(TARGET_HOME)/lib.bin/$(MACH)/top

gen.c: $(SRC_DIR)/table
	$(UTIL_HOME)/lib.bin/topgen $(SRC_DIR)/table

lint: $(CFILES) gen.c
	$(LINT) $(LINTFLAGS) $(CFILES)

clean:
	rm -f *.$(SUF) gen.c gen.h top

top.$(SUF):		gen.h
top.$(SUF):		$(CDIR)/top.h
top.$(SUF):		$(CDIR)/queue.h
queue.$(SUF):	$(CDIR)/queue.h
