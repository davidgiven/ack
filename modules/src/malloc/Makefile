EMHOME = ../../..
INSTALL = $(EMHOME)/modules/install
COMPARE = $(EMHOME)/modules/compare
CFLAGS = -O -I$(EMHOME)/modules/h $(COPT)
SUF = o
RCC = cc	#$ must produce a runnable object

MALLOCSRC =	READ_ME size_type.h gensize_type.h param.h impl.h check.h log.h phys.h \
		mal.c log.c phys.c check.c

.SUFFIXES: .$(SUF)
.c.$(SUF):
	$(CC) -c $(CFLAGS) $*.c

all:		malloc.$(SUF)

install:	all
		$(INSTALL) lib/malloc.$(SUF)

cmp:		all
		-$(COMPARE) lib/malloc.$(SUF)

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
		rm -f *.$(SUF) clashes malloc1.c gensize_type.h getsize malloc.c

gensize_type.h:	getsize
		getsize > gensize_type.h

getsize:	getsize.c
		$(RCC) -o getsize getsize.c
