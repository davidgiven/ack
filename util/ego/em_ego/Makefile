EMHOME=../../..
EMH=$(EMHOME)/h
EMLIB=$(EMHOME)/lib
MODLIB=$(EMHOME)/modules/lib
MODH=$(EMHOME)/modules/h
SHARE=../share
MODS=$(MODLIB)/libprint.a $(MODLIB)/libstring.a $(MODLIB)/libsystem.a

LDFLAGS=
CPPFLAGS=-DVERBOSE -DNOTCOMPACT -I$(EMH) -I$(MODH)
CFLAGS=$(CPPFLAGS) -O
LINTFLAGS=-hbu

all:	em_ego

em_ego:	em_ego.o
	$(CC) $(LDFLAGS) -o em_ego em_ego.o $(MODS)

install:	all
	rm -f $(EMLIB)/em_ego
	cp em_ego $(EMLIB)/em_ego

cmp:	all
	-cmp em_ego $(EMLIB)/em_ego

pr:
	@pr em_ego.c

opr:
	make pr | opr

lint:
	lint $(LINTFLAGS) $(CPPFLAGS) em_ego.c

clean:
	rm -f *.o em_ego
