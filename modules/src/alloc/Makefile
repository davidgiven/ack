EMHOME=../../..
HDIR = $(EMHOME)/modules/h
INSTALL=$(EMHOME)/modules/install
COMPARE=$(EMHOME)/modules/compare

CFLAGS = -O -I. -I$(HDIR)

SOURCES =	alloc.h\
		Malloc.c\
		botch.c\
		clear.c\
		st_alloc.c\
		std_alloc.c

OBJECTS =	botch.o clear.o st_alloc.o Malloc.o std_alloc.o

all:		liballoc.a

liballoc.a:	$(OBJECTS)
		ar cr liballoc.a $(OBJECTS)
		-sh -c 'ranlib liballoc.a'

install:	all
		$(INSTALL) lib/liballoc.a
		$(INSTALL) man/alloc.3
		$(INSTALL) h/alloc.h

cmp:		all
		$(COMPARE) lib/liballoc.a
		$(COMPARE) man/alloc.3
		$(COMPARE) h/alloc.h

pr:
		@pr Makefile $(SOURCES)

opr:
		make pr | opr

clean:
		rm -f *.[oa]

st_alloc.o:	alloc.h 
std_alloc.o:	alloc.h 
Malloc.o:	alloc.h
