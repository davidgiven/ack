EMHOME = ../../..
INSTALL = $(EMHOME)/modules/install
COMPARE = $(EMHOME)/modules/compare
CFLAGS = -O -I$(EMHOME)/modules/h

MALLOCSRC =	READ_ME size_type.h param.h impl.h check.h log.h phys.h \
		mal.c log.c phys.c check.c

all:		malloc.o

install:	all
		$(INSTALL) lib/malloc.o

cmp:		all
		$(COMPARE) lib/malloc.o

malloc1.c:	$(MALLOCSRC) Makefile add_file
		rm -f malloc1.c
		for i in $(MALLOCSRC) ; do add_file $$i >> malloc1.c ; done

malloc.c:	malloc1.c
		cclash -l7 -c malloc1.c > clashes
		cid -Fclashes < malloc1.c > malloc.c

pr:
		@pr Makefile add_file $(MALLOCSRC)

opr:
		make pr | opr

clean:
		rm -f *.o clashes malloc1.c size_type.h getsize malloc.c

size_type.h:	getsize
		getsize > size_type.h

getsize:	getsize.o
		$(CC) -o getsize getsize.o
