# $Header$

CFILES=cgg.c subr.c main.c coerc.c enterkeyw.c error.c emlookup.c expr.c instruct.c iocc.c lookup.c output.c set.c strlookup.c var.c hall.c
OFILES=cgg.o subr.o main.o coerc.o enterkeyw.o error.o emlookup.o expr.o instruct.o iocc.o lookup.o set.o strlookup.o var.o hall.o
SOURCES=*.h cgg.y scan.l cvtkeywords keywords coerc.c emlookup.c error.c expr.c hall.c instruct.c iocc.c lookup.c main.c output.c set.c strlookup.c subr.c var.c
EMHOME=../..
EMH=$(EMHOME)/h
MANDIR=$(EMHOME)/man
INCLUDES=-I$(EMH)
CFLAGS=-O $(INCLUDES)
YFLAGS=-v -d
LDFLAGS=
LEXLIB=-ll


cgg:	cgg.o $(OFILES) output.o
	$(CC) $(LDFLAGS) $(OFILES) output.o $(EMHOME)/lib/em_data.a $(LEXLIB) -o cgg

install:	cgg
	rm -f $(EMHOME)/lib/ncgg
	cp cgg $(EMHOME)/lib/ncgg
	rm -f $(MANDIR)/ncgg.6
	cp ncgg.6 $(MANDIR)/ncgg.6

cmp:	cgg
	-cmp cgg $(EMHOME)/lib/ncgg
	-cmp ncgg.6 $(MANDIR)/ncgg.6

debugcgg:	cgg.o $(OFILES) debugoutput.o
	$(CC) $(LDFLAGS) $(OFILES) debugoutput.o $(EMHOME)/lib/em_data.a -ll -o cgg

cgg.o:	scan.c

enterkeyw.c:	cvtkeywords keywords y.tab.h
	cvtkeywords keywords

debugoutput.o:	debugoutput.c
	$(CC) $(CFLAGS) -DCODEDEBUG -c debugoutput.c

debugoutput.c:	output.c
	cp output.c debugoutput.c

lint:	$(CFILES)
	lint $(INCLUDES) $(CFILES)
	touch lint

clean:
	rm -f cgg.c scan.c y.output y.tab.h enterkeyw.c
	rm -f $(OFILES) output.o debugoutput.o cgg lint

pr:
	pr $(SOURCES)

opr:
	-make pr|opr

depend:
	makedepend

cgg.o:	$(EMH)/cgg_cg.h
coerc.o:	$(EMH)/cgg_cg.h
debugoutput.o:	$(EMH)/cgg_cg.h
expr.o:	$(EMH)/cgg_cg.h
instruct.o:	$(EMH)/cgg_cg.h
iocc.o:	$(EMH)/cgg_cg.h
output.o:	$(EMH)/cgg_cg.h
set.o:	$(EMH)/cgg_cg.h
subr.o:	$(EMH)/cgg_cg.h
var.o:	$(EMH)/cgg_cg.h
# AUTOAUTOAUTOAUTOAUTOAUTOAUTOAUTOAUTOAUTOAUTOAUTO
cgg.o:	expr.h
cgg.o:	extern.h
cgg.o:	instruct.h
cgg.o:	iocc.h
cgg.o:	lookup.h
cgg.o:	param.h
cgg.o:	scan.c
cgg.o:	set.h
cgg.o:	varinfo.h
coerc.o:	assert.h
coerc.o:	extern.h
coerc.o:	iocc.h
coerc.o:	param.h
coerc.o:	property.h
coerc.o:	pseudo.h
coerc.o:	reg.h
coerc.o:	set.h
coerc.o:	token.h
coerc.o:	varinfo.h
debugoutput.o:	assert.h
debugoutput.o:	extern.h
debugoutput.o:	instruct.h
debugoutput.o:	lookup.h
debugoutput.o:	param.h
debugoutput.o:	property.h
debugoutput.o:	pseudo.h
debugoutput.o:	reg.h
debugoutput.o:	regvar.h
debugoutput.o:	set.h
debugoutput.o:	token.h
debugoutput.o:	varinfo.h
emlookup.o:	expr.h
emlookup.o:	param.h
enterkeyw.o:	lookup.h
expr.o:	assert.h
expr.o:	expr.h
expr.o:	extern.h
expr.o:	lookup.h
expr.o:	param.h
expr.o:	property.h
expr.o:	reg.h
expr.o:	regvar.h
expr.o:	set.h
expr.o:	token.h
hall.o:	assert.h
hall.o:	param.h
hall.o:	set.h
instruct.o:	expr.h
instruct.o:	extern.h
instruct.o:	instruct.h
instruct.o:	iocc.h
instruct.o:	param.h
instruct.o:	pseudo.h
instruct.o:	set.h
instruct.o:	varinfo.h
iocc.o:	assert.h
iocc.o:	expr.h
iocc.o:	extern.h
iocc.o:	iocc.h
iocc.o:	lookup.h
iocc.o:	param.h
iocc.o:	property.h
iocc.o:	regvar.h
iocc.o:	set.h
iocc.o:	token.h
lookup.o:	assert.h
lookup.o:	lookup.h
lookup.o:	param.h
output.o:	assert.h
output.o:	extern.h
output.o:	instruct.h
output.o:	lookup.h
output.o:	param.h
output.o:	property.h
output.o:	pseudo.h
output.o:	reg.h
output.o:	regvar.h
output.o:	set.h
output.o:	token.h
output.o:	varinfo.h
scan.o:	stdio.h
set.o:	extern.h
set.o:	lookup.h
set.o:	param.h
set.o:	property.h
set.o:	reg.h
set.o:	set.h
set.o:	token.h
strlookup.o:	param.h
subr.o:	expr.h
subr.o:	extern.h
subr.o:	instruct.h
subr.o:	lookup.h
subr.o:	param.h
subr.o:	property.h
subr.o:	reg.h
subr.o:	regvar.h
subr.o:	set.h
subr.o:	token.h
subr.o:	varinfo.h
tables.o:	data.h
tables.o:	param.h
tables.o:	tables.h
tables.o:	types.h
var.o:	instruct.h
var.o:	lookup.h
var.o:	param.h
var.o:	property.h
var.o:	reg.h
var.o:	set.h
var.o:	token.h
