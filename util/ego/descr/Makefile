EMHOME=../../..
EMH=$(EMHOME)/h
EMLIB=$(EMHOME)/lib
SHARE=../share
CPP=$(EMLIB)/cpp

LDFLAGS=-i
CPPFLAGS=-DVERBOSE -DNOTCOMPACT
CFLAGS=$(CPPFLAGS) -O
LINTFLAGS=-hbu

SOURCES = i86.descr m68k2.descr pdp.descr vax4.descr m68k4.descr m68020.descr \
	  em22.descr em24.descr em44.descr

TARGETS = i86descr m68k2descr vax4descr pdpdescr m68k4descr m68020descr \
	  em22descr em24descr em44descr

PRFILES = Makefile descr.sed $(SOURCES)

all:	$(TARGETS)

install:	all
	for i in $(TARGETS) ; do ../install $$i ; done

cmp:	all
	-for i in $(TARGETS) ; do ../compare $$i ; done

pr:
	@pr $(PRFILES)

opr:
	make pr | opr

clean:
	rm -f $(TARGETS) *.o Out out nohup.out

i86descr:	i86.descr descr.sed
		$(CPP) -P -I$(EMH) i86.descr | sed -f descr.sed > i86descr

pdpdescr:	pdp.descr descr.sed
		$(CPP) -P -I$(EMH) pdp.descr | sed -f descr.sed > pdpdescr

m68k2descr:	m68k2.descr descr.sed
		$(CPP) -P -I$(EMH) m68k2.descr | sed -f descr.sed > m68k2descr

m68k4descr:	m68k4.descr descr.sed
		$(CPP) -P -I$(EMH) m68k4.descr | sed -f descr.sed > m68k4descr

m68020descr:	m68020.descr descr.sed
		$(CPP) -P -I$(EMH) m68020.descr | sed -f descr.sed > m68020descr

vax4descr:	vax4.descr descr.sed
		$(CPP) -P -I$(EMH) vax4.descr | sed -f descr.sed > vax4descr

em22descr:	em22.descr descr.sed
		$(CPP) -P -I$(EMH) em22.descr | sed -f descr.sed > em22descr

em24descr:	em24.descr descr.sed
		$(CPP) -P -I$(EMH) em24.descr | sed -f descr.sed > em24descr

em44descr:	em44.descr descr.sed
		$(CPP) -P -I$(EMH) em44.descr | sed -f descr.sed > em44descr

sparcdescr:	sparc.descr descr.sed
		$(CPP) -P -I$(EMH) sparc.descr | sed -f descr.sed > sparcdescr

