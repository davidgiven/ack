EMHOME = ../../..
MODULES = $(EMHOME)/modules
INSTALL = $(MODULES)/install
COMPARE = $(MODULES)/compare
CFLAGS = -O $(COPT)
AR = ar
SUF = o
LIBSUF = a

OBJECTS =	AtEoIF.$(SUF)\
		AtEoIT.$(SUF)

.SUFFIXES: .$(SUF)
.c.$(SUF):
	$(CC) -c $(CFLAGS) $*.c

all:		libinput.$(LIBSUF)

libinput.$(LIBSUF):	$(OBJECTS)
		$(AR) cr libinput.$(LIBSUF) $(OBJECTS)
		-sh -c 'ranlib libinput.$(LIBSUF)'

install:	all
		$(INSTALL) lib/libinput.$(LIBSUF)
		$(INSTALL) man/input.3
		$(INSTALL) pkg/inp_pkg.body
		$(INSTALL) pkg/inp_pkg.spec

cmp:		all
		-$(COMPARE) lib/libinput.$(LIBSUF)
		-$(COMPARE) man/input.3
		-$(COMPARE) pkg/inp_pkg.body
		-$(COMPARE) pkg/inp_pkg.spec

pr:
		@pr Makefile inp_pkg.spec inp_pkg.body AtEoIF.c AtEoIT.c

opr:
		make pr | opr

clean:
		rm -f *.$(SUF) *.$(LIBSUF)

lintlib:
		lint -Cinput AtEoIF.c AtEoIT.c
		mv llib-linput.ln $(MODULES)/lib
