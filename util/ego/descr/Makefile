
EMHOME=../../..
EMH=$(EMHOME)/h
EMLIB=$(EMHOME)/lib
SHARE=../share

LDFLAGS=-i
CPPFLAGS=-DVERBOSE -DNOTCOMPACT
CFLAGS=$(CPPFLAGS) -O
LINTFLAGS=-phbac

SOURCES = i86.descr m68k2.descr pdp.descr vax2.descr vax4.descr

TARGETS = i86descr m68k2descr vax2descr vax4descr pdpdescr

PRFILES = Makefile descr.sed $(SOURCES)

all:	$(TARGETS)

install:	all
	../install i86descr
	../install m68k2descr
	../install vax2descr
	../install vax4descr
	../install pdpdescr

cmp:	all
	-../compare i86descr
	-../compare m68k2descr
	-../compare vax2descr
	-../compare vax4descr
	-../compare pdpdescr

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

vax2descr:	vax2.descr descr.sed
		$(CPP) -P -I$(EMH) vax2.descr | sed -f descr.sed > vax2descr

vax4descr:	vax4.descr descr.sed
		$(CPP) -P -I$(EMH) vax4.descr | sed -f descr.sed > vax4descr

