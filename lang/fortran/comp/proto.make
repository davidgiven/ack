# $Header$

# Makefile for f2c, a Fortran 77 to C converter

#PARAMS		do not remove this line!

UTIL_BIN = \
	$(UTIL_HOME)/bin
SRC_DIR = \
	$(SRC_HOME)/lang/fortran/comp
INCLUDES = -I$(SRC_DIR) -I.
CFLAGS = $(COPTIONS) $(INCLUDES)
LINTFLAGS = $(LINTOPTIONS) $(INCLUDES)
LDFLAGS = $(LDOPTIONS)

OBJECTS = main.$(SUF) init.$(SUF) gram.$(SUF) lex.$(SUF) proc.$(SUF) \
	  equiv.$(SUF) data.$(SUF) format.$(SUF) expr.$(SUF) exec.$(SUF) \
	  intr.$(SUF) io.$(SUF) misc.$(SUF) error.$(SUF) mem.$(SUF) \
	  names.$(SUF) output.$(SUF) p1output.$(SUF) pread.$(SUF) put.$(SUF) \
	  putpcc.$(SUF) vax.$(SUF) formatdata.$(SUF) parse_args.$(SUF) \
	  niceprintf.$(SUF) cds.$(SUF) sysdep.$(SUF) version.$(SUF)


GSRC = \
	$(SRC_DIR)/gram.head \
	$(SRC_DIR)/gram.dcl \
	$(SRC_DIR)/gram.expr \
	$(SRC_DIR)/gram.exec \
	$(SRC_DIR)/gram.io
CSRC = \
	$(SRC_DIR)/main.c \
	$(SRC_DIR)/init.c \
	$(SRC_DIR)/lex.c \
	$(SRC_DIR)/proc.c \
	$(SRC_DIR)/equiv.c \
	$(SRC_DIR)/data.c \
	$(SRC_DIR)/format.c \
	$(SRC_DIR)/expr.c \
	$(SRC_DIR)/exec.c \
	$(SRC_DIR)/intr.c \
	$(SRC_DIR)/io.c \
	$(SRC_DIR)/misc.c \
	$(SRC_DIR)/error.c \
	$(SRC_DIR)/mem.c \
	$(SRC_DIR)/names.c \
	$(SRC_DIR)/output.c \
	$(SRC_DIR)/p1output.c \
	$(SRC_DIR)/pread.c \
	$(SRC_DIR)/put.c \
	$(SRC_DIR)/putpcc.c \
	$(SRC_DIR)/vax.c \
	$(SRC_DIR)/formatdata.c \
	$(SRC_DIR)/parse_args.c \
	$(SRC_DIR)/niceprintf.c \
	$(SRC_DIR)/cds.c \
	$(SRC_DIR)/sysdep.c \
	$(SRC_DIR)/version.c
HSRC = \
	$(SRC_DIR)/defines.h \
	$(SRC_DIR)/defs.h \
	$(SRC_DIR)/f2c.h \
	$(SRC_DIR)/format.h \
	$(SRC_DIR)/ftypes.h \
	$(SRC_DIR)/iob.h \
	$(SRC_DIR)/machdefs.h \
	$(SRC_DIR)/names.h \
	$(SRC_DIR)/niceprintf.h \
	$(SRC_DIR)/output.h \
	$(SRC_DIR)/p1defs.h \
	$(SRC_DIR)/parse.h \
	$(SRC_DIR)/pccdefs.h \
	$(SRC_DIR)/sysdep.h \
	$(SRC_DIR)/usignal.h

SRC = 	$(SRC_DIR)/tokens $(GSRC) $(HSRC) $(CSRC)

CFILES = gram.c $(CSRC)

all:	f2c

install:	all
	cp f2c $(TARGET_HOME)/lib.bin/f2c
	if [ $(DO_MACHINE_INDEP) = y ] ; \
	then	mk_manpage $(SRC_DIR)/f2c.6 $(TARGET_HOME) ; \
	     	cp $(SRC_DIR)/f2c.h $(TARGET_HOME)/include/_tail_cc/f2c.h ; \
	fi

cmp:	all
	-cmp f2c $(TARGET_HOME)/lib.bin/f2c
	-cmp $(SRC_DIR)/f2c.h $(TARGET_HOME)/include/_tail_cc/f2c.h

lint:	$(CFILES) tokdefs.h
	$(LINT) $(LINTFLAGS) $(CFILES)

pr:
	@pr $(SRC_DIR)/proto.make $(SRC)

pr:
	make pr | opr

depend:	$(CFILES) tokdefs.h
	sed '/^#DEPENDENCIES/,$$d' Makefile >Makefile.new
	echo '#DEPENDENCIES' >>Makefile.new
	for i in $(CFILES) ; do \
		echo "`basename $$i .c`.$$(SUF):	$$i" >> Makefile.new ; \
		echo '	$$(CC) -c $$(CFLAGS)' $$i >> Makefile.new ; \
		$(UTIL_HOME)/lib.bin/cpp -d $(INCLUDES) $$i | sed "s/^/`basename $$i .c`.$$(SUF):	/" >> Makefile.new ; \
	done
	mv Makefile Makefile.old
	mv Makefile.new Makefile

f2c:	$(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) $(TARGET_HOME)/modules/lib/libstring.$(LIBSUF) -o f2c

gram.c:	$(GSRC) $(SRC_DIR)/defs.h tokdefs.h
	( sed <tokdefs.h "s/#define/%token/" ;\
		cat $(GSRC) ) >gram.in
	yacc gram.in
	echo "(expect 4 shift/reduce)"
	sed 's/^# line.*/\/* & *\//' y.tab.c >gram.c
	rm -f gram.in y.tab.c

tokdefs.h: $(SRC_DIR)/tokens
	grep -n . <$(SRC_DIR)/tokens | \
	   sed "s/\([^:]*\):\(.*\)/#define \2 \1/" >tokdefs.h

clean:
	rm -f gram.c *.$(SUF) f2c tokdefs.h Out

#DEPENDENCIES
gram.$(SUF):	gram.c
	$(CC) -c $(CFLAGS) gram.c
gram.$(SUF):	$(SRC_DIR)/p1defs.h
gram.$(SUF):	$(SRC_DIR)/machdefs.h
gram.$(SUF):	$(SRC_DIR)/defines.h
gram.$(SUF):	$(SRC_DIR)/ftypes.h
gram.$(SUF):	$(SRC_DIR)/sysdep.h
gram.$(SUF):	$(SRC_DIR)/defs.h
main.$(SUF):	$(SRC_DIR)/main.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/main.c
main.$(SUF):	$(SRC_DIR)/parse.h
main.$(SUF):	$(SRC_DIR)/machdefs.h
main.$(SUF):	$(SRC_DIR)/defines.h
main.$(SUF):	$(SRC_DIR)/ftypes.h
main.$(SUF):	$(SRC_DIR)/sysdep.h
main.$(SUF):	$(SRC_DIR)/defs.h
init.$(SUF):	$(SRC_DIR)/init.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/init.c
init.$(SUF):	$(SRC_DIR)/iob.h
init.$(SUF):	$(SRC_DIR)/niceprintf.h
init.$(SUF):	$(SRC_DIR)/output.h
init.$(SUF):	$(SRC_DIR)/machdefs.h
init.$(SUF):	$(SRC_DIR)/defines.h
init.$(SUF):	$(SRC_DIR)/ftypes.h
init.$(SUF):	$(SRC_DIR)/sysdep.h
init.$(SUF):	$(SRC_DIR)/defs.h
lex.$(SUF):	$(SRC_DIR)/lex.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/lex.c
lex.$(SUF):	$(SRC_DIR)/p1defs.h
lex.$(SUF):	./tokdefs.h
lex.$(SUF):	$(SRC_DIR)/machdefs.h
lex.$(SUF):	$(SRC_DIR)/defines.h
lex.$(SUF):	$(SRC_DIR)/ftypes.h
lex.$(SUF):	$(SRC_DIR)/sysdep.h
lex.$(SUF):	$(SRC_DIR)/defs.h
proc.$(SUF):	$(SRC_DIR)/proc.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/proc.c
proc.$(SUF):	$(SRC_DIR)/p1defs.h
proc.$(SUF):	$(SRC_DIR)/niceprintf.h
proc.$(SUF):	$(SRC_DIR)/output.h
proc.$(SUF):	$(SRC_DIR)/names.h
proc.$(SUF):	$(SRC_DIR)/machdefs.h
proc.$(SUF):	$(SRC_DIR)/defines.h
proc.$(SUF):	$(SRC_DIR)/ftypes.h
proc.$(SUF):	$(SRC_DIR)/sysdep.h
proc.$(SUF):	$(SRC_DIR)/defs.h
equiv.$(SUF):	$(SRC_DIR)/equiv.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/equiv.c
equiv.$(SUF):	$(SRC_DIR)/machdefs.h
equiv.$(SUF):	$(SRC_DIR)/defines.h
equiv.$(SUF):	$(SRC_DIR)/ftypes.h
equiv.$(SUF):	$(SRC_DIR)/sysdep.h
equiv.$(SUF):	$(SRC_DIR)/defs.h
data.$(SUF):	$(SRC_DIR)/data.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/data.c
data.$(SUF):	$(SRC_DIR)/machdefs.h
data.$(SUF):	$(SRC_DIR)/defines.h
data.$(SUF):	$(SRC_DIR)/ftypes.h
data.$(SUF):	$(SRC_DIR)/sysdep.h
data.$(SUF):	$(SRC_DIR)/defs.h
format.$(SUF):	$(SRC_DIR)/format.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/format.c
format.$(SUF):	$(SRC_DIR)/iob.h
format.$(SUF):	$(SRC_DIR)/names.h
format.$(SUF):	$(SRC_DIR)/niceprintf.h
format.$(SUF):	$(SRC_DIR)/output.h
format.$(SUF):	$(SRC_DIR)/format.h
format.$(SUF):	$(SRC_DIR)/p1defs.h
format.$(SUF):	$(SRC_DIR)/machdefs.h
format.$(SUF):	$(SRC_DIR)/defines.h
format.$(SUF):	$(SRC_DIR)/ftypes.h
format.$(SUF):	$(SRC_DIR)/sysdep.h
format.$(SUF):	$(SRC_DIR)/defs.h
expr.$(SUF):	$(SRC_DIR)/expr.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/expr.c
expr.$(SUF):	$(SRC_DIR)/names.h
expr.$(SUF):	$(SRC_DIR)/niceprintf.h
expr.$(SUF):	$(SRC_DIR)/output.h
expr.$(SUF):	$(SRC_DIR)/machdefs.h
expr.$(SUF):	$(SRC_DIR)/defines.h
expr.$(SUF):	$(SRC_DIR)/ftypes.h
expr.$(SUF):	$(SRC_DIR)/sysdep.h
expr.$(SUF):	$(SRC_DIR)/defs.h
exec.$(SUF):	$(SRC_DIR)/exec.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/exec.c
exec.$(SUF):	$(SRC_DIR)/names.h
exec.$(SUF):	$(SRC_DIR)/p1defs.h
exec.$(SUF):	$(SRC_DIR)/machdefs.h
exec.$(SUF):	$(SRC_DIR)/defines.h
exec.$(SUF):	$(SRC_DIR)/ftypes.h
exec.$(SUF):	$(SRC_DIR)/sysdep.h
exec.$(SUF):	$(SRC_DIR)/defs.h
intr.$(SUF):	$(SRC_DIR)/intr.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/intr.c
intr.$(SUF):	$(SRC_DIR)/names.h
intr.$(SUF):	$(SRC_DIR)/machdefs.h
intr.$(SUF):	$(SRC_DIR)/defines.h
intr.$(SUF):	$(SRC_DIR)/ftypes.h
intr.$(SUF):	$(SRC_DIR)/sysdep.h
intr.$(SUF):	$(SRC_DIR)/defs.h
io.$(SUF):	$(SRC_DIR)/io.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/io.c
io.$(SUF):	$(SRC_DIR)/iob.h
io.$(SUF):	$(SRC_DIR)/names.h
io.$(SUF):	$(SRC_DIR)/machdefs.h
io.$(SUF):	$(SRC_DIR)/defines.h
io.$(SUF):	$(SRC_DIR)/ftypes.h
io.$(SUF):	$(SRC_DIR)/sysdep.h
io.$(SUF):	$(SRC_DIR)/defs.h
misc.$(SUF):	$(SRC_DIR)/misc.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/misc.c
misc.$(SUF):	$(SRC_DIR)/machdefs.h
misc.$(SUF):	$(SRC_DIR)/defines.h
misc.$(SUF):	$(SRC_DIR)/ftypes.h
misc.$(SUF):	$(SRC_DIR)/sysdep.h
misc.$(SUF):	$(SRC_DIR)/defs.h
error.$(SUF):	$(SRC_DIR)/error.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/error.c
error.$(SUF):	$(SRC_DIR)/machdefs.h
error.$(SUF):	$(SRC_DIR)/defines.h
error.$(SUF):	$(SRC_DIR)/ftypes.h
error.$(SUF):	$(SRC_DIR)/sysdep.h
error.$(SUF):	$(SRC_DIR)/defs.h
mem.$(SUF):	$(SRC_DIR)/mem.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/mem.c
mem.$(SUF):	$(SRC_DIR)/iob.h
mem.$(SUF):	$(SRC_DIR)/machdefs.h
mem.$(SUF):	$(SRC_DIR)/defines.h
mem.$(SUF):	$(SRC_DIR)/ftypes.h
mem.$(SUF):	$(SRC_DIR)/sysdep.h
mem.$(SUF):	$(SRC_DIR)/defs.h
names.$(SUF):	$(SRC_DIR)/names.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/names.c
names.$(SUF):	$(SRC_DIR)/iob.h
names.$(SUF):	$(SRC_DIR)/names.h
names.$(SUF):	$(SRC_DIR)/niceprintf.h
names.$(SUF):	$(SRC_DIR)/output.h
names.$(SUF):	$(SRC_DIR)/machdefs.h
names.$(SUF):	$(SRC_DIR)/defines.h
names.$(SUF):	$(SRC_DIR)/ftypes.h
names.$(SUF):	$(SRC_DIR)/sysdep.h
names.$(SUF):	$(SRC_DIR)/defs.h
output.$(SUF):	$(SRC_DIR)/output.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/output.c
output.$(SUF):	$(SRC_DIR)/niceprintf.h
output.$(SUF):	$(SRC_DIR)/output.h
output.$(SUF):	$(SRC_DIR)/names.h
output.$(SUF):	$(SRC_DIR)/machdefs.h
output.$(SUF):	$(SRC_DIR)/defines.h
output.$(SUF):	$(SRC_DIR)/ftypes.h
output.$(SUF):	$(SRC_DIR)/sysdep.h
output.$(SUF):	$(SRC_DIR)/defs.h
p1output.$(SUF):	$(SRC_DIR)/p1output.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/p1output.c
p1output.$(SUF):	$(SRC_DIR)/names.h
p1output.$(SUF):	$(SRC_DIR)/niceprintf.h
p1output.$(SUF):	$(SRC_DIR)/output.h
p1output.$(SUF):	$(SRC_DIR)/p1defs.h
p1output.$(SUF):	$(SRC_DIR)/machdefs.h
p1output.$(SUF):	$(SRC_DIR)/defines.h
p1output.$(SUF):	$(SRC_DIR)/ftypes.h
p1output.$(SUF):	$(SRC_DIR)/sysdep.h
p1output.$(SUF):	$(SRC_DIR)/defs.h
pread.$(SUF):	$(SRC_DIR)/pread.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/pread.c
pread.$(SUF):	$(SRC_DIR)/machdefs.h
pread.$(SUF):	$(SRC_DIR)/defines.h
pread.$(SUF):	$(SRC_DIR)/ftypes.h
pread.$(SUF):	$(SRC_DIR)/sysdep.h
pread.$(SUF):	$(SRC_DIR)/defs.h
put.$(SUF):	$(SRC_DIR)/put.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/put.c
put.$(SUF):	$(SRC_DIR)/p1defs.h
put.$(SUF):	$(SRC_DIR)/pccdefs.h
put.$(SUF):	$(SRC_DIR)/names.h
put.$(SUF):	$(SRC_DIR)/machdefs.h
put.$(SUF):	$(SRC_DIR)/defines.h
put.$(SUF):	$(SRC_DIR)/ftypes.h
put.$(SUF):	$(SRC_DIR)/sysdep.h
put.$(SUF):	$(SRC_DIR)/defs.h
putpcc.$(SUF):	$(SRC_DIR)/putpcc.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/putpcc.c
putpcc.$(SUF):	$(SRC_DIR)/p1defs.h
putpcc.$(SUF):	$(SRC_DIR)/names.h
putpcc.$(SUF):	$(SRC_DIR)/niceprintf.h
putpcc.$(SUF):	$(SRC_DIR)/output.h
putpcc.$(SUF):	$(SRC_DIR)/pccdefs.h
putpcc.$(SUF):	$(SRC_DIR)/machdefs.h
putpcc.$(SUF):	$(SRC_DIR)/defines.h
putpcc.$(SUF):	$(SRC_DIR)/ftypes.h
putpcc.$(SUF):	$(SRC_DIR)/sysdep.h
putpcc.$(SUF):	$(SRC_DIR)/defs.h
vax.$(SUF):	$(SRC_DIR)/vax.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/vax.c
vax.$(SUF):	$(SRC_DIR)/niceprintf.h
vax.$(SUF):	$(SRC_DIR)/output.h
vax.$(SUF):	$(SRC_DIR)/pccdefs.h
vax.$(SUF):	$(SRC_DIR)/machdefs.h
vax.$(SUF):	$(SRC_DIR)/defines.h
vax.$(SUF):	$(SRC_DIR)/ftypes.h
vax.$(SUF):	$(SRC_DIR)/sysdep.h
vax.$(SUF):	$(SRC_DIR)/defs.h
formatdata.$(SUF):	$(SRC_DIR)/formatdata.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/formatdata.c
formatdata.$(SUF):	$(SRC_DIR)/format.h
formatdata.$(SUF):	$(SRC_DIR)/names.h
formatdata.$(SUF):	$(SRC_DIR)/niceprintf.h
formatdata.$(SUF):	$(SRC_DIR)/output.h
formatdata.$(SUF):	$(SRC_DIR)/machdefs.h
formatdata.$(SUF):	$(SRC_DIR)/defines.h
formatdata.$(SUF):	$(SRC_DIR)/ftypes.h
formatdata.$(SUF):	$(SRC_DIR)/sysdep.h
formatdata.$(SUF):	$(SRC_DIR)/defs.h
parse_args.$(SUF):	$(SRC_DIR)/parse_args.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/parse_args.c
parse_args.$(SUF):	$(SRC_DIR)/parse.h
niceprintf.$(SUF):	$(SRC_DIR)/niceprintf.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/niceprintf.c
niceprintf.$(SUF):	$(SRC_DIR)/niceprintf.h
niceprintf.$(SUF):	$(SRC_DIR)/output.h
niceprintf.$(SUF):	$(SRC_DIR)/names.h
niceprintf.$(SUF):	$(SRC_DIR)/machdefs.h
niceprintf.$(SUF):	$(SRC_DIR)/defines.h
niceprintf.$(SUF):	$(SRC_DIR)/ftypes.h
niceprintf.$(SUF):	$(SRC_DIR)/sysdep.h
niceprintf.$(SUF):	$(SRC_DIR)/defs.h
cds.$(SUF):	$(SRC_DIR)/cds.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/cds.c
cds.$(SUF):	$(SRC_DIR)/sysdep.h
sysdep.$(SUF):	$(SRC_DIR)/sysdep.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/sysdep.c
sysdep.$(SUF):	$(SRC_DIR)/usignal.h
sysdep.$(SUF):	$(SRC_DIR)/machdefs.h
sysdep.$(SUF):	$(SRC_DIR)/defines.h
sysdep.$(SUF):	$(SRC_DIR)/ftypes.h
sysdep.$(SUF):	$(SRC_DIR)/sysdep.h
sysdep.$(SUF):	$(SRC_DIR)/defs.h
version.$(SUF):	$(SRC_DIR)/version.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/version.c
memset.$(SUF):	$(SRC_DIR)/memset.c
	$(CC) -c $(CFLAGS) $(SRC_DIR)/memset.c
