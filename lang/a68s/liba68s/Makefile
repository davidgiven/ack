EMROOT=../../..
ACK=$(EMROOT)/bin/$(MACH)
PC=/proj/em/bin/sun3 -.p -PR$(EMROOT)/lang/a68s/cpem/cpem -Ras=/proj/em/Work/lib/m68020/as -I/proj/em/h -Rbe=/proj/em/Work/lib/m68020/cg
PCFLAGS=-v -L -e -LIB # -Oego -SR -CJ -BO -SP
EPCFLAGS=-v -L -e -LIB $(BSD4) $(VAX4)
UTIL=$(EMROOT)/lang/a68s/util
TAILOR=$(UTIL)/tailor
XREF=$(UTIL)/xref -i$(UTIL)/pascal.ign -p
CHECKSEQ=$(UTIL)/checkseq
TERRS=/dev/tty
TNOS=101 2 103 104 105 111 21 122 123 124 125 32 41 150 151 152 153 154 155 161 $(RECIPE)
CFILES=aclose.c aopen.c powi.c powr.c mod.c entier.c signi.c signr.c \
	timesten.c shl.c shr.c time.c sin.c cos.c arctan.c sqrt.c exp.c \
	ln.c maxr.c cleanup.c
COFILES=aclose.o aopen.o powi.o powr.o mod.o entier.o signi.o signr.o \
	timesten.o shl.o shr.o time.o sin.o cos.o arctan.o sqrt.o exp.o \
	ln.o maxr.o cleanup.o
FILES=run68g.p
GFILES=errorr.p global.p safeaccess.p 
GOFILES=errorr.o global.o safeaccess.o
PFILES=collp.p colltm.p collts.p complex.p crmult.p crrefn.p dclpsn.p drefm.p \
	drefs.p dummy.p genrec.p getmult.p getout.p gtot.p gtotref.p \
	gvasstx.p gvscope.p heapmul.p heapstr.p is.p linit2.p linit34.p \
	linitinc.p nassts.p nassp.p pcollmul.p pcollst.p rangent.p rangext.p \
	rnstart.p routn.p routnp.p rowm.p rownm.p scopext.p selectr.p \
	selecttsn.p setcc.p skip.p slice12.p slicen.p strsubtrim.p \
	structscope.p tassp.p tasstm.p tassts.p trim.p widchar.p widen.p 
POFILES=collp.o colltm.o collts.o complex.o crmult.o crrefn.o dclpsn.o drefm.o \
	drefs.o dummy.o genrec.o getmult.o getout.o gtot.o gtotref.o gvasstx.o \
	gvscope.o heapmul.o heapstr.o is.o linit2.o linit34.o linitinc.o \
	nassts.o nassp.o pcollmul.o pcollst.o rangent.o rangext.o rnstart.o \
	routn.o routnp.o rowm.o rownm.o scopext.o selectr.o selecttsn.o \
	setcc.o skip.o slice12.o slicen.o strsubtrim.o structscope.o tassp.o \
	tasstm.o tassts.o trim.o widchar.o widen.o
OPFILES=catpl.p cfstr.p mulis.p powneg.p uplwb.p uplwbm.p uplwbmstr.p
OPOFILES=catpl.o cfstr.o mulis.o powneg.o uplwb.o uplwbm.o uplwbmstr.o
SPFILES=bytespack.p random.p trig.p
SPOFILES=bytespack.o random.o trig.o
TFILES=associate.p dumbacch.p duminch.p dumoutch.p ensure.p fixed.p float.p \
	gett.p newline.p onend.p openclose.p posenq.p putt.p reset.p sett.p \
	space.p standass.p standin.p standout.p stbacch.p stinch.p stopen.p \
	stoutch.p whole.p
TOFILES=associate.o dumbacch.o duminch.o dumoutch.o ensure.o fixed.o float.o \
	gett.o newline.o onend.o openclose.o posenq.o putt.o reset.o sett.o \
	space.o standass.o standin.o standout.o stbacch.o stinch.o stopen.o \
	stoutch.o whole.o
EFILES=calls.e chains.e div.e get.e getaddr.e globale.e hoist.e put.e \
	standback.e swap.e trace.e wrs.e
EOFILES=calls.o chains.o div.o get.o getaddr.o globale.o hoist.o put.o \
	standback.o swap.o trace.o wrs.o
LIBFILES=$(GFILES) $(PFILES) $(OPFILES) $(SPFILES) $(TFILES)
LIBOFILES=$(GOFILES) $(POFILES) $(OPOFILES) $(SPOFILES) $(TOFILES)

all:		liba68s$(w)$(p)

rundecs.h:	check$(w)$(p) rundecs.p
		echo $(TNOS) 300 | $(TAILOR) rundecs.p $(TERRS)  \
			>rundecs.h

rundecsg.h:	check$(w)$(p) rundecs.p
		echo $(TNOS) 71 300 | $(TAILOR) rundecs.p $(TERRS) >rundecsg.h

run68g.o:	rundecsg.h run68g.p
		(cat rundecsg.h; \
		 cat run68g.p ) \
		    >temp.p
		$(PC) $(PCFLAGS) -c.s temp.p
		sed -e '/^.define __m_a_i_n/d' -e '/^.extern __m_a_i_n/,$$d' -e '/^.globl	__m_a_i_n/,$$d' temp.s > run68g.s
		$(PC) $(PCFLAGS) -c.o run68g.s
		rm temp.p run68g.s

.p.o:
		( echo $(TNOS) 300 | $(TAILOR) $*.p $(TERRS) ) \
		    >temp.p
		$(PC) $(PCFLAGS) -c.s temp.p
		mv temp.s $*.s
		$(PC) $(PCFLAGS) -c.o $*.s
		rm temp.p $*.s

$(LIBOFILES):	rundecs.h

.SUFFIXES:	.e

e.h:		check$(w)$(p)

.e.o:
		$(PC) $(EPCFLAGS) -c.s -DEM_WSIZE=$(w) -DEM_PSIZE=$(p) $*.e
		$(PC) $(EPCFLAGS) -c.o $*.s
		rm $*.s

$(EOFILES):	e.h

maxr.o:		maxr.c
		/lib/cpp <maxr.c >temp.c
		$(PC) $(PCFLAGS) -I$(EMROOT)/h -c.s -o maxr.s temp.c
		$(PC) $(PCFLAGS) -c.o maxr.s
		rm maxr.s

.c.o:
		$(PC) $(PCFLAGS) -I$(EMROOT)/h -c.s $*.c
		$(PC) $(PCFLAGS) -c.o $*.s
		rm $*.s

liba68s:	liba68s$(w)$(p)

liba68s$(w)$(p): $(EOFILES) $(COFILES) $(LIBOFILES) $(SOFILES) run68g.o
		-rm liba68s$(w)$(p)
		$(ASAR) crv liba68s$(w)$(p) $(EOFILES) $(COFILES) $(LIBOFILES) $(SOFILES) run68g.o
		sh -c '$${RANLIB-:} liba68s$(w)$(p)'

check$(w)$(p):
		/bin/make -f $(EMROOT)/lang/a68s/liba68s/Makefile clean
		echo >> check$(w)$(p)

checkseq:
		$(CHECKSEQ) rundecs.p $(LIBFILES)

pr:
		pr rundecs.p $(LIBFILES) $(FILES) $(EFILES) $(CFILES)

xref:
		(/bin/make pr; \
		 echo 1000 | $(TAILOR) rundecs.p $(TERRS) | $(XREF) | pr -h rundecs.xref; \
		 for II in $(LIBFILES); do echo 1000 | $(TAILOR) $$II $(TERRS); done \
			| $(XREF) | pr -h run68.xref \
		) | opr

clean:		
		-rm liba68s$(w)$(p) check?? rundec*.h *.o

