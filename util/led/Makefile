#
#	Author: L.J. Bekema @ VU Informatica, Amsterdam
#

#	Definitions for the making programs.

EMHOME = ../..
LIBDIR= $(EMHOME)/lib
MODLIBDIR = $(EMHOME)/modules/lib
PREFLAGS=  -I$(EMHOME)/h -DNDEBUG -DNASSERT
CFLAGS	= $(PREFLAGS) -O
LDFLAGS	= 
LDLIBS = $(MODLIBDIR)/libstring.a $(MODLIBDIR)/libobject.a
LINTFLAGS=-phbxa $(PREFLAGS)
PR	= pr
PRFLAGS	=

#	Some convenient macro definitions.

CFILES	= archive.c error.c extract.c finish.c main.c memory.c\
	  output.c read.c relocate.c save.c scan.c sym.c write.c
HFILES	= assert.h const.h debug.h defs.h memory.h orig.h scan.h
OFILES	= archive.o error.o extract.o finish.o main.o memory.o\
	  output.o read.o relocate.o save.o scan.o sym.o write.o

#	Things that can be made.

led:	$(OFILES)
	$(CC) $(LDFLAGS) $(OFILES) $(LDLIBS) -o led

install:led
	rm -f $(LIBDIR)/em_led $(EMHOME)/man/led.6 $(EMHOME)/man/ack.out.5
	cp led $(LIBDIR)/em_led
	cp led.6 $(EMHOME)/man/led.6
	cp ack.out.5 $(EMHOME)/man/ack.out.5

cmp:	led
	cmp led $(LIBDIR)/em_led

lint:
	lint $(LINTFLAGS) $(CFILES)

pr:	$(CFILES) $(HFILES) mach.c
	$(PR) $(PRFLAGS) $?
	@touch pr
opr:	
	make pr | opr

clean:
	rm -f Out *.o led nohup.out

depend:
	makedepend $(CFILES)

#	The next lines are generated automatically.
# AUTOAUTOAUTOAUTOAUTOAUTOAUTOAUTOAUTOAUTOAUTOAUTOAUTOAUTOAUTOAUTOAUTOAUTOAUTO
archive.o:	const.h
archive.o:	debug.h
archive.o:	defs.h
archive.o:	memory.h
error.o:	const.h
extract.o:	const.h
extract.o:	debug.h
extract.o:	defs.h
extract.o:	memory.h
extract.o:	orig.h
extract.o:	scan.h
finish.o:	const.h
finish.o:	defs.h
finish.o:	memory.h
finish.o:	orig.h
finish.o:	scan.h
main.o:	const.h
main.o:	debug.h
main.o:	defs.h
main.o:	memory.h
main.o:	orig.h
memory.o:	assert.h
memory.o:	const.h
memory.o:	debug.h
memory.o:	mach.c
memory.o:	memory.h
output.o:	const.h
output.o:	memory.h
read.o:	assert.h
read.o:	const.h
relocate.o:	const.h
relocate.o:	debug.h
relocate.o:	defs.h
relocate.o:	orig.h
save.o:	assert.h
save.o:	const.h
save.o:	memory.h
scan.o:	assert.h
scan.o:	const.h
scan.o:	memory.h
scan.o:	scan.h
sym.o:	const.h
sym.o:	memory.h
write.o:	assert.h
write.o:	const.h
write.o:	memory.h
write.o:	orig.h
