EMBIN =	../../bin
CFLAGS = -O
LDFLAGS =
TARGETS = mkdep cid cclash

all:	$(TARGETS)

install:	all
	cp $(TARGETS) $(EMBIN)

clean:
	rm -f *.o $(TARGETS)

mkdep:	mkdep.o
	$(CC) $(LDFLAGS) -o mkdep mkdep.o

cid:	cid.o
	$(CC) $(LDFLAGS) -o cid cid.o

cclash:	cclash.o
	$(CC) $(LDFLAGS) -o cclash cclash.o
