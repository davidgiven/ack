# $Header$

#PARAMS		do not remove this line!

#MACH_DEFINE	do not remove this or the next line!
MACH = 

h	= $(TARGET_HOME)/h
LIBOBJ	= $(TARGET_HOME)/modules/lib/libobject.$(LIBSUF)
CDIR	= $(SRC_HOME)/mach/proto/as
CPP	= $(UTIL_HOME)/lib.bin/cpp
DEF	=
MDIR	= $(SRC_HOME)/mach/$(MACH)/as

FFLAG	=
INCL	= -I$(CDIR) -I$(MDIR) -I$h $(DEF)
CFLAGS	= $(FFLAG) $(INCL) $(COPTIONS)
YFLAGS	= -d
LDFLAGS	= $(FFLAG) $(LDOPTIONS)

CSRC	= $(CDIR)/comm3.c $(CDIR)/comm4.c $(CDIR)/comm5.c \
	  $(CDIR)/comm6.c $(CDIR)/comm7.c $(CDIR)/comm8.c
COBJ	= comm3.$(SUF) comm4.$(SUF) comm5.$(SUF) \
	  comm6.$(SUF) comm7.$(SUF) comm8.$(SUF)
MACHSRC	= $(MDIR)/mach0.c $(MDIR)/mach1.c $(MDIR)/mach2.c \
	  $(MDIR)/mach3.c $(MDIR)/mach4.c $(MDIR)/mach5.c
COMM	= $(CDIR)/comm0.h $(CDIR)/comm1.h $(CDIR)/comm2.y $(CSRC)

all:	as

install:all
	-mkdir $(TARGET_HOME)/lib.bin/$(MACH)
	cp as $(TARGET_HOME)/lib.bin/$(MACH)/as

cmp:
	-cmp as $(TARGET_HOME)/lib.bin/$(MACH)/as

clean:
	rm -f *.$(SUF) as as.[cy] y.tab.h

pr:	$(MACHSRC)
	@pr -n $(MACHSRC)

opr:
	make pr | opr
as:	$(COBJ) as.$(SUF)
	$(CC) $(LDFLAGS) $(COBJ) as.$(SUF) $(LIBOBJ) -o as

as.y:	$(CDIR)/comm2.y
	$(CPP) -P -I$(MDIR) -I$h $(DEF) $(CDIR)/comm2.y >as.y
	-@if test -f $(MDIR)/Expect ; then cat $(MDIR)/Expect ; else echo "expect 1 shift/reduce conflict"; fi

lint:   $(CSRC) as.c
	$(LINT) $(LINTFLAGS) $(CSRC) as.c

as.c:	as.y
	yacc $(YFLAGS) as.y && mv y.tab.c as.c

y.tab.h:	as.c
$(COBJ):	y.tab.h
$(COBJ) as.y:	$(CDIR)/comm0.h $(MDIR)/mach0.c
$(COBJ) as.y:	$(CDIR)/comm1.h $(MDIR)/mach1.c
as.y:		$(MDIR)/mach2.c
comm3.$(SUF):	$(MDIR)/mach3.c
as.y:		$(MDIR)/mach4.c
comm8.$(SUF):	$(MDIR)/mach5.c
comm3.$(SUF):	$(CDIR)/comm3.c
		$(CC) -c $(CFLAGS) $(CDIR)/comm3.c
comm4.$(SUF):	$(CDIR)/comm4.c
		$(CC) -c $(CFLAGS) $(CDIR)/comm4.c
comm5.$(SUF):	$(CDIR)/comm5.c
		$(CC) -c $(CFLAGS) $(CDIR)/comm5.c
comm6.$(SUF):	$(CDIR)/comm6.c
		$(CC) -c $(CFLAGS) $(CDIR)/comm6.c
comm7.$(SUF):	$(CDIR)/comm7.c
		$(CC) -c $(CFLAGS) $(CDIR)/comm7.c
comm8.$(SUF):	$(CDIR)/comm8.c
		$(CC) -c $(CFLAGS) $(CDIR)/comm8.c
