# $Header$

#PARAMS		do not remove this line!

SRC_DIR =	$(SRC_HOME)/modules/src/em_opt
MOD_DIR =	$(TARGET_HOME)/modules

# set HOWMUCH to head -20 to limit number of patterns used
#HOWMUCH =	head -20
HOWMUCH	=	cat

INCLUDES =	-I$(TARGET_HOME)/h -I$(MOD_DIR)/h \
		-I$(MOD_DIR)/pkg -I$(SRC_DIR) -I.
UINCLUDES =	-I$(UTIL_HOME)/h -I$(UTIL_HOME)/modules/h \
		-I$(UTIL_HOME)/modules/pkg -I$(SRC_DIR) -I.
PREFLAGS =	$(INCLUDES) -DPRIVATE=static#	 -DDEBUG
UPREFLAGS =	$(UINCLUDES) -DPRIVATE=static#	 -DDEBUG

# Enable the next line to produce a version that output's the line number
# from the patterns file each time an optimization is performed.
#PREFLAGS =	$(PREFLAGS) -DSTATS

CFLAGS =	$(PREFLAGS) $(COPTIONS)
LINTFLAGS =	$(PREFLAGS) -DNORCSID
LDFLAGS =	$(LDOPTIONS)
UCFLAGS =	$(UPREFLAGS) $(UCOPTIONS)
ULDFLAGS =	$(ULDOPTIONS)
LLOPT =
CMD =		'$(CC) -c $(CFLAGS)'

LIBOPT =	libemopt.$(LIBSUF)
LIBCEOPT =	libCEopt.$(LIBSUF)

.SUFFIXES:	.d .r

.r.d:		; CMD=$(CMD); export CMD; awk -f $(SRC_DIR)/makefuns.awk $*.r | sh -x
		touch $@

.SUFFIXES: .$(SUF)
.c.$(SUF):
	$(CC) -c $(CFLAGS) $*.c

CSRC =		$(SRC_DIR)/main.c $(SRC_DIR)/nopt.c $(SRC_DIR)/mkstrct.c \
		$(SRC_DIR)/aux.c $(SRC_DIR)/outputdfa.c $(SRC_DIR)/outcalls.c\
		$(SRC_DIR)/findworst.c $(SRC_DIR)/initlex.c

SRCS =		$(SRC_DIR)/proto.make $(SRC_DIR)/nopt.h $(SRC_DIR)/parser.h \
		$(SRC_DIR)/parser.g $(SRC_DIR)/syntax.l $(SRC_DIR)/pseudo.r\
		$(SRC_DIR)/patterns $(CSRC)

NOFILES =	nopt.$(SUF) dfa.$(SUF) trans.$(SUF) aux.$(SUF) mkstrct.$(SUF)

POFILES =	parser.$(USUF) syntax.$(USUF) outputdfa.$(USUF) outcalls.$(USUF)\
		findworst.$(USUF) initlex.$(USUF) Lpars.$(USUF)

GENFILES =	Lpars.h Lpars.c parserdummy parser.c syntax.c \
		dfa.c dfa.c.new trans.c trans.c.new\
		incalls.d incalls.r incalls.r.new pseudo.d pseudo.r

all:		em_nopt $(LIBOPT) $(LIBCEOPT)

install:	all
		cp $(LIBOPT) $(MOD_DIR)/lib/$(LIBOPT)
		$(RANLIB) $(MOD_DIR)/lib/$(LIBOPT)
		cp $(LIBCEOPT) $(MOD_DIR)/lib/$(LIBCEOPT)
		$(RANLIB) $(MOD_DIR)/lib/$(LIBCEOPT)
		cp $(SRC_DIR)/em_opt.3 $(MOD_DIR)/man/em_opt.3
		cp em_nopt $(TARGET_HOME)/lib.bin/em_nopt
		if [ $(DO_MACHINE_INDEP) = y ] ; \
		then	mk_manpage $(SRC_DIR)/em_opt.3 $(TARGET_HOME) ; \
			mk_manpage $(SRC_DIR)/em_nopt.6 $(TARGET_HOME) ; \
		fi

cmp:		all
		-cmp $(LIBOPT) $(MOD_DIR)/lib/$(LIBOPT)
		-cmp $(LIBCEOPT) $(MOD_DIR)/lib/$(LIBCEOPT)
		-cmp em_nopt $(TARGET_HOME)/lib.bin/em_nopt
		-cmp $(SRC_DIR)/em_opt.3 $(MOD_DIR)/man/em_opt.3

pr:
		@pr $(SRCS)

opr:
		make pr | opr

lint:		lintparser lintnopt

clean:
		rm -f O_*.$(SUF)
		rm -f O_*.c
		rm -f $(NOFILES) main.$(SUF) $(POFILES)
		rm -f $(GENFILES) parser em_nopt $(LIBOPT) $(LIBCEOPT)

# How to build stand alone version of the optimizer

NOPTLIB = 	$(MOD_DIR)/lib/libread_emk.$(LIBSUF)\
		$(MOD_DIR)/lib/libemk.$(LIBSUF)\
		$(MOD_DIR)/lib/liballoc.$(LIBSUF)\
		$(MOD_DIR)/lib/malloc.$(SUF)\
		$(MOD_DIR)/lib/libprint.$(LIBSUF)\
		$(MOD_DIR)/lib/libstring.$(LIBSUF)\
		$(MOD_DIR)/lib/libsystem.$(LIBSUF)\
		$(TARGET_HOME)/lib.bin/em_data.$(LIBSUF)

em_nopt:	incalls.r main.$(SUF) $(NOFILES)
		$(CC) $(LDFLAGS) -o em_nopt main.$(SUF) $(NOFILES) $(NOPTLIB)

OLINT =		$(SRC_DIR)/main.c $(SRC_DIR)/mkstrct.c $(SRC_DIR)/nopt.c $(SRC_DIR)/aux.c dfa.c trans.c

OLINTLIB = 	$(UTIL_HOME)/modules/lib/$(LINTPREF)read_emkV.$(LINTSUF)\
		$(UTIL_HOME)/modules/lib/$(LINTPREF)emk.$(LINTSUF)\
		$(UTIL_HOME)/modules/lib/$(LINTPREF)system.$(LINTSUF)\
		$(UTIL_HOME)/modules/lib/$(LINTPREF)alloc.$(LINTSUF)\
		$(UTIL_HOME)/modules/lib/$(LINTPREF)print.$(LINTSUF)\
		$(UTIL_HOME)/modules/lib/$(LINTPREF)string.$(LINTSUF)

lintnopt:	incalls.r $(OLINT)
		$(LINT) $(LINTFLAGS) $(OLINT) $(OLINTLIB)

# How to build the library version of the optimizer

$(LIBOPT):	$(NOFILES) pseudo.d incalls.d
		rm -f $(LIBOPT)
		$(AR) r $(LIBOPT) O_*.$(SUF) $(NOFILES)
		$(RANLIB) $(LIBOPT)

$(LIBCEOPT):	incalls.r
		rm -f O_*.$(SUF) $(NOFILES) pseudo.d incalls.d
		-mv $(LIBOPT) $(LIBOPT).saved
		make PREFLAGS='$(INCLUDES) -DPRIVATE=static -DCODE_EXPANDER' $(LIBOPT)
		mv $(LIBOPT) $(LIBCEOPT)
		-mv $(LIBOPT).saved $(LIBOPT)
		rm -f O_*.$(SUF) $(NOFILES)

incalls.r:	$(SRC_DIR)/patterns parser
		-$(UTIL_HOME)/lib.bin/cpp $(SRC_DIR)/patterns | $(HOWMUCH) >/tmp/patts
		parser </tmp/patts
		-rm /tmp/patts

# How to build the lint library for the optimizer

LLINT =		$(SRC_DIR)/nopt.c $(SRC_DIR)/mkstrct.c $(SRC_DIR)/aux.c dfa.c trans.c

lintlib:	incalls.r $(LLINT)
		$(MK_LINT_LIB) emopt $(MOD_DIR)/lib $(LLINT)

# How to build program to parse patterns table and build c files.

PARSERLIB =	$(UTIL_HOME)/lib.bin/em_data.$(ULIBSUF)\
		$(UTIL_HOME)/modules/lib/liballoc.$(ULIBSUF)\
		$(UTIL_HOME)/modules/lib/libstring.$(ULIBSUF)\
		$(UTIL_HOME)/modules/lib/libsystem.$(ULIBSUF)

parser:		parserdummy $(POFILES) $(PARSERLIB)
		$(UCC) -o parser $(ULDFLAGS) $(POFILES) $(PARSERLIB)

syntax.c:	$(SRC_DIR)/syntax.l
		flex -ts $(SRC_DIR)/syntax.l > syntax.c

parserdummy:	$(SRC_DIR)/parser.g
		LLgen $(LLOPT) $(SRC_DIR)/parser.g
		touch parserdummy

PLINT =		parser.c syntax.c $(SRC_DIR)/outputdfa.c $(SRC_DIR)/outcalls.c\
		$(SRC_DIR)/findworst.c $(SRC_DIR)/initlex.c Lpars.c

PLINTLIB =	$(UTIL_HOME)/modules/lib/$(LINTPREF)alloc.$(LINTSUF)\
		$(UTIL_HOME)/modules/lib/$(LINTPREF)string.$(LINTSUF)\
		$(UTIL_HOME)/modules/lib/$(LINTPREF)system.$(LINTSUF)

LINTFLAGS =	$(LINTOPTIONS)

lintparser:	parserdummy $(PLINT)
		$(LINT) $(LINTFLAGS) $(PLINT) $(PLINTLIB)

# Dependancies

main.$(SUF):	$(SRC_DIR)/main.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/main.c
main.$(SUF):	$(SRC_DIR)/nopt.h

nopt.$(SUF):	$(SRC_DIR)/nopt.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/nopt.c
nopt.$(SUF):	$(SRC_DIR)/nopt.h

mkstrct.$(SUF):	$(SRC_DIR)/mkstrct.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/mkstrct.c
mkstrct.$(SUF):	$(SRC_DIR)/nopt.h

aux.$(SUF):	$(SRC_DIR)/aux.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/aux.c
aux.$(SUF):	$(SRC_DIR)/nopt.h

dfa.$(SUF):	dfa.c
		$(CC) -c $(CFLAGS) dfa.c
dfa.$(SUF):	$(SRC_DIR)/nopt.h

trans.$(SUF):	trans.c
		$(CC) -c $(CFLAGS) trans.c
trans.$(SUF):	$(SRC_DIR)/nopt.h

incalls.d:	$(SRC_DIR)/nopt.h $(SRC_DIR)/makefuns.awk
pseudo.d:	$(SRC_DIR)/nopt.h $(SRC_DIR)/makefuns.awk
pseudo.r:	$(SRC_DIR)/pseudo.r
		cp $(SRC_DIR)/pseudo.r pseudo.r

parser.$(USUF):	parser.c
		$(UCC) -c $(UCFLAGS) parser.c
parser.$(USUF):	Lpars.h $(SRC_DIR)/parser.h

Lpars.$(USUF):	Lpars.c
		$(UCC) -c $(UCFLAGS) Lpars.c
Lpars.$(USUF):	Lpars.h

syntax.$(USUF):	syntax.c
		$(UCC) -c $(UCFLAGS) syntax.c
sybtax.$(USUF):	$(SRC_DIR)/parser.h Lpars.h

outputdfa.$(USUF):	$(SRC_DIR)/outputdfa.c
		$(UCC) -c $(UCFLAGS) $(SRC_DIR)/outputdfa.c
outputdfa.$(USUF):	$(SRC_DIR)/parser.h Lpars.h

outcalls.$(USUF):	$(SRC_DIR)/outcalls.c
		$(UCC) -c $(UCFLAGS) $(SRC_DIR)/outcalls.c
outcalls.$(USUF):	$(SRC_DIR)/parser.h

findworst.$(USUF):	$(SRC_DIR)/findworst.c
		$(UCC) -c $(UCFLAGS) $(SRC_DIR)/findworst.c
findworst.$(USUF):	$(SRC_DIR)/parser.h

initlex.$(USUF):	$(SRC_DIR)/initlex.c
		$(UCC) -c $(UCFLAGS) $(SRC_DIR)/initlex.c
initlex.$(USUF):	$(SRC_DIR)/parser.h
