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

PREFLAGS= -DNDEBUG -DNASSERT
CFLAGS	= -O $(PREFLAGS)
LDFLAGS	=
LINTFLAGS=-phbxac $(PREFLAGS)
PR	= pr
PRFLAGS	=

#	Some convenient macro definitions.

CFILES	= archive.c byte_order.c error.c extract.c finish.c main.c memory.c\
	  output.c read.c relocate.c save.c scan.c sym.c write.c
CVFILES	= archive.c,v byte_order.c,v error.c,v extract.c,v finish.c,v main.c,v\
	  memory.c,v output.c,v read.c,v relocate.c,v save.c,v scan.c,v\
	  sym.c,v write.c,v
HFILES	= arch.h assert.h const.h debug.h defs.h memory.h orig.h out.h\
	  ranlib.h scan.h
HVFILES	= arch.h,v assert.h,v const.h,v debug.h,v defs.h,v memory.h,v orig.h,v\
	  out.h,v ranlib.h,v scan.h,v
OFILES	= archive.o byte_order.o error.o extract.o finish.o main.o memory.o\
	  output.o read.o relocate.o save.o scan.o sym.o write.o

#	Things that can be made.

led:	$(OFILES)
	$(CC) $(LDFLAGS) $(OFILES) $(LDLIBS) -o led

mach.c:	mach.c,v
	co -q mach.c

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

depend:
	-for i in $(CFILES); do\
		if test ! -f $$i; then\
			echo $$i >> checked.out;\
			co -q $$i;\
		fi;\
	done
	makedepend $(CFILES)
	rm -f `cat checked.out` checked.out

#	The next lines are generated automatically.
# AUTOAUTOAUTOAUTOAUTOAUTOAUTOAUTOAUTOAUTOAUTOAUTOAUTOAUTOAUTOAUTOAUTOAUTOAUTO
archive.o:	arch.h
archive.o:	const.h
archive.o:	debug.h
archive.o:	defs.h
archive.o:	memory.h
archive.o:	out.h
archive.o:	ranlib.h
byte_order.o:	assert.h
byte_order.o:	const.h
error.o:	const.h
error.o:	out.h
extract.o:	const.h
extract.o:	debug.h
extract.o:	defs.h
extract.o:	memory.h
extract.o:	orig.h
extract.o:	out.h
extract.o:	scan.h
finish.o:	const.h
finish.o:	defs.h
finish.o:	memory.h
finish.o:	orig.h
finish.o:	out.h
finish.o:	scan.h
main.o:	const.h
main.o:	debug.h
main.o:	defs.h
main.o:	memory.h
main.o:	orig.h
main.o:	out.h
memory.o:	assert.h
memory.o:	const.h
memory.o:	debug.h
memory.o:	mach.c
memory.o:	memory.h
memory.o:	out.h
output.o:	const.h
output.o:	memory.h
output.o:	out.h
read.o:	arch.h
read.o:	assert.h
read.o:	const.h
read.o:	out.h
read.o:	ranlib.h
relocate.o:	const.h
relocate.o:	debug.h
relocate.o:	defs.h
relocate.o:	orig.h
relocate.o:	out.h
save.o:	arch.h
save.o:	assert.h
save.o:	const.h
save.o:	memory.h
save.o:	out.h
scan.o:	arch.h
scan.o:	assert.h
scan.o:	const.h
scan.o:	memory.h
scan.o:	out.h
scan.o:	ranlib.h
scan.o:	scan.h
sym.o:	const.h
sym.o:	memory.h
sym.o:	out.h
write.o:	assert.h
write.o:	const.h
write.o:	memory.h
write.o:	orig.h
write.o:	out.h
