EMHOME=../../..
MODULES=$(EMHOME)/modules
HDIR =	$(MODULES)/h
INCLUDES = -I$(HDIR)
CFLAGS = $(INCLUDES) -O
INSTALL = $(MODULES)/install
COMPARE = $(MODULES)/compare
AR = ar

all:		libassert.a

libassert.a:	BadAssert.o
		$(AR) r libassert.a BadAssert.o
		-sh -c 'ranlib libassert.a'

install:	all
		$(INSTALL) lib/libassert.a
		$(INSTALL) man/assert.3
		$(INSTALL) h/assert.h

cmp:		all
		$(COMPARE) lib/libassert.a
		$(COMPARE) man/assert.3
		$(COMPARE) h/assert.h

pr:
		@pr Makefile assert.h BadAssert.c

opr:
		make pr | opr

clean:
		rm -f *.[oa]

lintlib:
		lint $(INCLUDES) -Cassert BadAssert.c
		mv llib-lassert.ln $(MODULES)/lib
