EMHOME=../../..
HDIR = $(EMHOME)/modules/h
INSTALL=$(EMHOME)/modules/install
COMPARE=$(EMHOME)/modules/compare
INCLUDES = -I. -I$(HDIR)
AR = ar
SUF = o
LIBSUF = a

CFLAGS = -O $(INCLUDES) $(COPT)

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

OBJECTS =	botch.$(SUF) clear.$(SUF) st_alloc.$(SUF) Malloc.$(SUF) \
		Salloc.$(SUF) \
		Srealloc.$(SUF) Realloc.$(SUF) std_alloc.$(SUF) No_Mem.$(SUF)

.SUFFIXES: .$(SUF)
.c.$(SUF):
	$(CC) -c $(CFLAGS) $*.c

all:		liballoc.$(LIBSUF)

liballoc.$(LIBSUF):	$(OBJECTS)
		$(AR) cr liballoc.$(LIBSUF) $(OBJECTS)
		-sh -c 'ranlib liballoc.$(LIBSUF)'

install:	all
		$(INSTALL) lib/liballoc.$(LIBSUF)
		$(INSTALL) man/alloc.3
		$(INSTALL) h/alloc.h

cmp:		all
		-$(COMPARE) lib/liballoc.$(LIBSUF)
		-$(COMPARE) man/alloc.3
		-$(COMPARE) h/alloc.h

pr:
		@pr Makefile $(SOURCES)

opr:
		make pr | opr

clean:
		rm -f *.$(SUF) *.$(LIBSUF)

lintlib:
		lint $(INCLUDES) -Calloc $(CSRC)
		mv llib-lalloc.ln $(EMHOME)/modules/lib

st_alloc.$(SUF):	alloc.h 
std_alloc.$(SUF):	alloc.h 
Malloc.$(SUF):		alloc.h
