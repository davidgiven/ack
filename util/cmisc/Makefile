EM = ../..
EMBIN =	$(EM)/bin
EMMAN = $(EM)/man
CFLAGS = -O
LDFLAGS =
TARGETS = mkdep cid cclash prid

all:	mkdep cid cclash prid

install:	all
	rm -f $(EMBIN)/mkdep $(EMBIN)/cid $(EMBIN)/cclash $(EMBIN)/prid
	cp mkdep cid cclash prid $(EMBIN)
	rm -f $(EMMAN)/mkdep.1 $(EMMAN)/cid.1 $(EMMAN)/cclash.1 $(EMMAN)/prid.1
	cp mkdep cid cclash prid $(EMBIN)
	cp mkdep.1 cid.1 cclash.1 prid.1 $(EMMAN)

cmp:	all
	-cmp mkdep $(EMBIN)/mkdep
	-cmp cid $(EMBIN)/cid
	-cmp cclash $(EMBIN)/cclash
	-cmp prid $(EMBIN)/prid
	-cmp mkdep.1 $(EMMAN)/mkdep.1
	-cmp cid.1 $(EMMAN)/cid.1
	-cmp cclash.1 $(EMMAN)/cclash.1
	-cmp prid.1 $(EMMAN)/prid.1

clean:
	rm -f *.o mkdep cid cclash prid

mkdep:	mkdep.o
	$(CC) $(LDFLAGS) -o mkdep mkdep.o

cid:	cid.o GCIPM.o
	$(CC) $(LDFLAGS) -o cid cid.o GCIPM.o

cclash:	cclash.o GCIPM.o
	$(CC) $(LDFLAGS) -o cclash cclash.o GCIPM.o

prid:	prid.o GCIPM.o
	$(CC) $(LDFLAGS) -o prid prid.o GCIPM.o
