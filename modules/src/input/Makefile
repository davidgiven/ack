EMHOME = ../../..
MODULES = $(EMHOME)/modules
INSTALL = $(MODULES)/install
COMPARE = $(MODULES)/compare
CFLAGS = -O

OBJECTS =	AtEoIF.o\
		AtEoIT.o

all:		libinput.a

libinput.a:	$(OBJECTS)
		ar cr libinput.a $(OBJECTS)
		-sh -c 'ranlib libinput.a'

install:	all
		$(INSTALL) lib/libinput.a
		$(INSTALL) man/input.3
		$(INSTALL) pkg/inp_pkg.body
		$(INSTALL) pkg/inp_pkg.spec

cmp:		all
		$(COMPARE) lib/libinput.a
		$(COMPARE) man/input.3
		$(COMPARE) pkg/inp_pkg.body
		$(COMPARE) pkg/inp_pkg.spec

pr:
		@pr Makefile inp_pkg.spec inp_pkg.body AtEoIF.c AtEoIT.c

opr:
		make pr | opr

clean:
		rm -f *.[oa]

lintlib:
		lint -Cinput AtEoIF.c AtEoIT.c
		mv llib-linput.ln $(MODULES)/lib
