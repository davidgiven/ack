#
#	Author: L.J. Bekema @ VU Informatica, Amsterdam
#

.SUFFIXES:	.c,v .h .h,v

.c,v.o:
	co -q $*.c
	$(CC) $(CFLAGS) -c $*.c
	rm -f $*.c

.h,v.h:
	co -q $*.h

#	Definitions for the making programs.

M	= /user1/sjoerd/amoeba/libmonix
X	= /usr/em/lib/l68k2
CC	= l68k2
LIBDIR	= ../../lib
PREFLAGS= -DNDEBUG # -DNASSERT
CFLAGS	= -L $(PREFLAGS)
LDFLAGS	= -b0:0x20000 -a0:2 -a1:2 -a2:2 -a3:2 -a4:2 -c.out $M/head.o
LDLIBS	= $X/tail_cc.1s $X/tail_cc.2g $M/libmonix.a $X/tail_em.rt $X/end_em
LINTFLAGS=-phbxac $(PREFLAGS)
PR	= pr
PRFLAGS	=

#	Some convenient macro definitions.

CFILES	= archive.c byte_order.c error.c extract.c finish.c main.c memory.c\
	  output.c read.c relocate.c save.c scan.c sym.c write.c
CVFILES	= archive.c,v byte_order.c,v error.c,v extract.c,v finish.c,v main.c,v\
	  memory.c,v output.c,v read.c,v relocate.c,v save.c,v scan.c,v\
	  sym.c,v write.c,v
HFILES	= assert.h const.h debug.h defs.h memory.h orig.h scan.h
HVFILES	= assert.h,v const.h,v debug.h,v defs.h,v memory.h,v orig.h,v scan.h,v
OFILES	= archive.o byte_order.o error.o extract.o finish.o main.o memory.o\
	  output.o read.o relocate.o save.o scan.o sym.o write.o

#	Things that can be made.

led:	$(OFILES)
	$(CC) $(LDFLAGS) $(OFILES) $(LDLIBS) -o led.led
	/user1/sjoerd/amoeba/util/convert led.led led
	/user1/sjoerd/amoeba/test/load led >/user1/sjoerd/amoeba/test/led.load

mach.c:	mach.c,v
	co -q mach.c

install:led
	cp led $(LIBDIR)/em_led

cmp:	led
	cmp led $(LIBDIR)/em_led

lint:
	-for i in $(CFILES) $(HFILES) mach.c; do\
		if test ! -f $$i; then\
			echo $$i >> checked.out;\
			co -q $$i;\
		fi;\
	done
	lint $(LINTFLAGS) $(CFILES)
	rm -f `cat checked.out` checked.out

pr:	$(CVFILES) $(HVFILES) mach.c
	@-for i in $?; do\
		co -q -p $$i | $(PR) $(PRFLAGS) -h `basename $$i ,v`;\
	done
	@touch pr
opr:	
	make pr | opr

clean:
	rm *.o led

depend:
	-for i in $(CFILES); do\
		if test ! -f $$i; then\
			echo $$i >> checked.out;\
			co -q $$i;\
		fi;\
	done
	makedepend $(CFILES)
	rm -f `cat checked.out` checked.out

procentry.o:
	$(CC) -c -L procentry.c

#	The next lines are generated automatically.
# AUTOAUTOAUTOAUTOAUTOAUTOAUTOAUTOAUTOAUTOAUTOAUTOAUTOAUTOAUTOAUTOAUTOAUTOAUTO
archive.o:	../../h/arch.h
archive.o:	../../h/out.h
archive.o:	../../h/ranlib.h
archive.o:	const.h
archive.o:	debug.h
archive.o:	defs.h
archive.o:	memory.h
byte_order.o:	assert.h
byte_order.o:	const.h
error.o:	../../h/out.h
error.o:	const.h
extract.o:	../../h/out.h
extract.o:	const.h
extract.o:	debug.h
extract.o:	defs.h
extract.o:	memory.h
extract.o:	orig.h
extract.o:	scan.h
finish.o:	../../h/out.h
finish.o:	const.h
finish.o:	defs.h
finish.o:	memory.h
finish.o:	orig.h
finish.o:	scan.h
main.o:	../../h/out.h
main.o:	const.h
main.o:	debug.h
main.o:	defs.h
main.o:	memory.h
main.o:	orig.h
memory.o:	../../h/out.h
memory.o:	assert.h
memory.o:	const.h
memory.o:	debug.h
memory.o:	mach.c
memory.o:	memory.h
output.o:	../../h/out.h
output.o:	const.h
output.o:	memory.h
read.o:	../../h/arch.h
read.o:	../../h/out.h
read.o:	../../h/ranlib.h
read.o:	assert.h
read.o:	const.h
relocate.o:	../../h/out.h
relocate.o:	const.h
relocate.o:	debug.h
relocate.o:	defs.h
relocate.o:	orig.h
save.o:	../../h/arch.h
save.o:	../../h/out.h
save.o:	assert.h
save.o:	const.h
save.o:	memory.h
scan.o:	../../h/arch.h
scan.o:	../../h/out.h
scan.o:	../../h/ranlib.h
scan.o:	assert.h
scan.o:	const.h
scan.o:	memory.h
scan.o:	scan.h
sym.o:	../../h/out.h
sym.o:	const.h
sym.o:	memory.h
write.o:	../../h/out.h
write.o:	assert.h
write.o:	const.h
write.o:	memory.h
write.o:	orig.h
