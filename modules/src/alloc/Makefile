EMHOME=../../..
HDIR = $(EMHOME)/modules/h
INSTALL=$(EMHOME)/modules/install
COMPARE=$(EMHOME)/modules/compare
INCLUDES = -I. -I$(HDIR)
AR = ar

CFLAGS = -O $(INCLUDES)

CSRC =		Malloc.c\
		Salloc.c\
		Srealloc.c\
		Realloc.c\
		botch.c\
		clear.c\
		st_alloc.c\
		std_alloc.c \
		No_Mem.c
SOURCES =	alloc.h\
		$(CSRC)

OBJECTS =	botch.o clear.o st_alloc.o Malloc.o Salloc.o \
		Srealloc.o Realloc.o std_alloc.o No_Mem.o

all:		liballoc.a

liballoc.a:	$(OBJECTS)
		$(AR) cr liballoc.a $(OBJECTS)
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

lintlib:
		lint $(INCLUDES) -Calloc $(CSRC)
		mv llib-lalloc.ln $(EMHOME)/modules/lib
st_alloc.o:	alloc.h 
std_alloc.o:	alloc.h 
Malloc.o:	alloc.h
