
EMHOME=../../..
EMH=$(EMHOME)/h
EMLIB=$(EMHOME)/lib
SHARE=../share

LDFLAGS=
CPPFLAGS=-DVERBOSE -DNOTCOMPACT
CFLAGS=$(CPPFLAGS) -O
LINTFLAGS=-hbu

all:

install:
	rm -f $(EMLIB)/em_ego
	cp em_ego $(EMLIB)/em_ego

cmp:	all
	-cmp em_ego $(EMLIB)/em_ego

pr:
	@pr em_ego

opr:
	make pr | opr

clean:

