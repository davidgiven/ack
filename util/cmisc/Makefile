EM = ../..
EMBIN =	$(EM)/bin
CFLAGS = -O
LDFLAGS =
TARGETS = mkdep cid cclash prid

all:	$(TARGETS)

install:	all
	cp $(TARGETS) $(EMBIN)

clean:
	rm -f *.o $(TARGETS)

mkdep:	mkdep.o
	$(CC) $(LDFLAGS) -o mkdep mkdep.o

cid:	cid.o GCIPM.o
	$(CC) $(LDFLAGS) -o cid cid.o GCIPM.o

cclash:	cclash.o GCIPM.o
	$(CC) $(LDFLAGS) -o cclash cclash.o GCIPM.o

prid:	prid.o GCIPM.o
	$(CC) $(LDFLAGS) -o prid prid.o GCIPM.o
