EMHOME=../../..
MODULES=$(EMHOME)/modules
HDIR =	$(MODULES)/h
CFLAGS = -I$(HDIR) -O
INSTALL = $(MODULES)/install
COMPARE = $(MODULES)/compare

all:		libassert.a

libassert.a:	BadAssertion.o
		ar r libassert.a BadAssertion.o
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
		@pr Makefile assert.h BadAssertion.c

opr:
		make pr | opr

clean:
		rm -f *.[oa]
