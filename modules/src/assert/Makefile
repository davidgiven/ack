EMHOME=../../..
MODULES=$(EMHOME)/modules
HDIR =	$(MODULES)/h
INCLUDES = -I$(HDIR)
CFLAGS = $(INCLUDES) -O $(COPT)
INSTALL = $(MODULES)/install
COMPARE = $(MODULES)/compare
AR = ar
SUF = o
LIBSUF = a

.SUFFIXES: .$(SUF)
.c.$(SUF):
	$(CC) -c $(CFLAGS) $*.c

all:		libassert.$(LIBSUF)

libassert.$(LIBSUF):	BadAssert.$(SUF)
		$(AR) r libassert.$(LIBSUF) BadAssert.$(SUF)
		-sh -c 'ranlib libassert.$(LIBSUF)'

install:	all
		$(INSTALL) lib/libassert.$(LIBSUF)
		$(INSTALL) man/assert.3
		$(INSTALL) h/assert.h

cmp:		all
		-$(COMPARE) lib/libassert.$(LIBSUF)
		-$(COMPARE) man/assert.3
		-$(COMPARE) h/assert.h

pr:
		@pr Makefile assert.h BadAssert.c

opr:
		make pr | opr

clean:
		rm -f *.$(LIBSUF) *.$(SUF)

lintlib:
		lint $(INCLUDES) -Cassert BadAssert.c
		mv llib-lassert.ln $(MODULES)/lib
