EM=../../..
PREFLAGS=-I.
PFLAGS=
CFLAGS=$(PREFLAGS) $(PFLAGS) -O
LDFLAGS=-i $(PFLAGS)
LINTOPTS=-hbxac
CDIR=$(EM)/mach/proto/top
CFILES=$(CDIR)/top.c $(CDIR)/queue.c gen.c
OFILES=top.o queue.o gen.o

all:	gen.c
	make top

top:	$(OFILES)
	$(CC) $(LDFLAGS) $(OFILES)  $(LIBS) -o top

top.o:	$(CDIR)/top.c
	$(CC) -c $(CFLAGS) $(CDIR)/top.c

queue.o: $(CDIR)/queue.c
	$(CC) -c $(CFLAGS) $(CDIR)/queue.c

install: all
	$(EM)/mach/install top

cmp:	 all
	-$(EM)/mach/compare top

gen.c: table
	$(EM)/lib/topgen table

lint: $(CFILES)
	lint $(LINTOPTS) $(PREFLAGS) $(CFILES)

clean:
	rm -f *.o gen.c gen.h top

top.o:		gen.h
top.o:		$(CDIR)/top.h
top.o:		$(CDIR)/queue.h
queue.o:	$(CDIR)/queue.h
