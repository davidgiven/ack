EMHOME=../../..
HDIR = $(EMHOME)/modules/h
INSTALL=$(EMHOME)/modules/install
COMPARE=$(EMHOME)/modules/compare

CFLAGS = -O -I$(HDIR)

SOURCES =	Xmalloc.c

OBJECTS =	Xmalloc.o

all:		$(OBJECTS)

install:	all
		$(INSTALL) lib/Xmalloc.o

compare:	all
		$(COMPARE) lib/Xmalloc.o

clean:
		rm -f *.[oa]
