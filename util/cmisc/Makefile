EM = ../..
EMBIN =	$(EM)/bin
EMMAN = $(EM)/man
CFLAGS = -O
LDFLAGS =

all:	mkdep cid cclash prid tabgen

install:	all
	rm -f $(EMBIN)/mkdep $(EMBIN)/cid $(EMBIN)/cclash $(EMBIN)/prid \
		$(EMBIN)/tabgen
	cp mkdep cid cclash prid tabgen $(EMBIN)
	rm -f $(EMMAN)/mkdep.1 $(EMMAN)/cid.1 $(EMMAN)/cclash.1 \
		$(EMMAN)/prid.1 $(EMMAN)/tabgen.1
	cp mkdep.1 cid.1 cclash.1 prid.1 tabgen.1 $(EMMAN)

cmp:	all
	-cmp mkdep $(EMBIN)/mkdep
	-cmp cid $(EMBIN)/cid
	-cmp cclash $(EMBIN)/cclash
	-cmp prid $(EMBIN)/prid
	-cmp tabgen $(EMBIN)/tabgen
	-cmp mkdep.1 $(EMMAN)/mkdep.1
	-cmp cid.1 $(EMMAN)/cid.1
	-cmp cclash.1 $(EMMAN)/cclash.1
	-cmp prid.1 $(EMMAN)/prid.1
	-cmp tabgen.1 $(EMMAN)/tabgen.1

clean:
	rm -f *.o mkdep cid cclash prid tabgen

pr:
	@pr `pwd`/Makefile `pwd`/mkdep.c `pwd`/cclash.c `pwd`/cid.c \
		`pwd`/prid.c `pwd`/GCIPM.c `pwd`/tabgen.c

opr:
	make pr | opr

mkdep:	mkdep.o
	$(CC) $(LDFLAGS) -o mkdep mkdep.o

tabgen:	tabgen.o
	$(CC) $(LDFLAGS) -o tabgen tabgen.o

cid:	cid.o GCIPM.o
	$(CC) $(LDFLAGS) -o cid cid.o GCIPM.o

cclash:	cclash.o GCIPM.o
	$(CC) $(LDFLAGS) -o cclash cclash.o GCIPM.o

prid:	prid.o GCIPM.o
	$(CC) $(LDFLAGS) -o prid prid.o GCIPM.o
