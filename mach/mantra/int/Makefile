INTS=em2_t--- em2_---- em2_t-cp em2_t-c- em2_t--p \
em4_t--- em4_---- em4_t-cp em4_t-c- em4_t--p
P = mloop
FILES=deffile $P0 $P1 $P2 $P3 $P4 $P5 $P6 $P7 $P8 $P9 $Pa $Pb $Pc
CPP=../../../lib/cpp
b=../../../lib/em
FLTRAP=

all:	$(INTS) em

em:	em.c
	$(CC) -o em -I../../../h em.c

tmp.s:	$(FILES)
	cat $(FILES) > tmp.s

em2_t---:	tmp.s compile con_float.o
	./compile -o em2_t--- -P $(FLTRAP) -Dopfreq=0 -Dlast=1 -Dcount=0 -Dtest=1 -Dflow=0 -Dprof=0 tmp.s con_float.o

em2_----:	tmp.s compile con_float.o
	./compile -o em2_---- -P $(FLTRAP) -Dopfreq=0 -Dlast=0 -Dcount=0 -Dtest=0 -Dflow=0 -Dprof=0 tmp.s con_float.o

em2_t-cp:	tmp.s compile con_float.o
	./compile -o em2_t-cp -P $(FLTRAP) -Dopfreq=0 -Dlast=1 -Dcount=1 -Dtest=1 -Dflow=0 -Dprof=1 tmp.s con_float.o

em2_t-c-:	tmp.s compile con_float.o
	./compile -o em2_t-c- -P $(FLTRAP) -Dopfreq=0 -Dlast=1 -Dcount=1 -Dtest=1 -Dflow=0 -Dprof=0 tmp.s con_float.o

em2_t--p:	tmp.s compile con_float.o
	./compile -o em2_t--p -P $(FLTRAP) -Dopfreq=0 -Dlast=1 -Dcount=0 -Dtest=1 -Dflow=0 -Dprof=1 tmp.s con_float.o

em4_t---:	tmp.s compile con_float.o
	./compile -o em4_t--- -P $(FLTRAP) -Dlword -Dopfreq=0 -Dlast=1 -Dcount=0 -Dtest=1 -Dflow=0 -Dprof=0 tmp.s con_float.o

em4_----:	tmp.s compile con_float.o
	./compile -o em4_---- -P $(FLTRAP) -Dlword -Dopfreq=0 -Dlast=0 -Dcount=0 -Dtest=0 -Dflow=0 -Dprof=0 tmp.s con_float.o

em4_t-cp:	tmp.s compile con_float.o
	./compile -o em4_t-cp -P $(FLTRAP) -Dlword -Dopfreq=0 -Dlast=1 -Dcount=1 -Dtest=1 -Dflow=0 -Dprof=1 tmp.s con_float.o

em4_t-c-:tmp.s compile con_float.o
	./compile -o em4_t-c- -P $(FLTRAP) -Dlword -Dopfreq=0 -Dlast=1 -Dcount=1 -Dtest=1 -Dflow=0 -Dprof=0 tmp.s con_float.o

em4_t--p:	tmp.s compile con_float.o
	./compile -o em4_t--p -P $(FLTRAP) -Dlword -Dopfreq=0 -Dlast=1 -Dcount=0 -Dtest=1 -Dflow=0 -Dprof=1 tmp.s con_float.o

con_float.o:	con_float.c compile
	./compile -c -L -O con_float.c

compile:	Makefile
		case `ack_sys` in sun*) MACH=sun2;; pmds*) MACH=pmds4;; \
				    m68_sysV_0) MACH=mantra;; \
				    m68k2) MACH=m68k2;; *) MACH=m68k4;; \
		esac ; echo $$MACH -r.c -.m -fp '$$*' > compile; chmod +x compile
install:	all
		-mkdir $b24 $b44
		ls em2_???? | sed 's:em2_\(.*\):cp & $b24/em_\1:' | sh
		ls em4_???? | sed 's:em4_\(.*\):cp & $b44/em_\1:' | sh
		cp em ../../../bin/em
		cp em.1 ../../../man/em.1

cmp: 		all
		ls em2_???? | sed 's:em2_\(.*\):cmp & $b24/em_\1:' | sh
		ls em4_???? | sed 's:em4_\(.*\):cmp & $b44/em_\1:' | sh
		-cmp em ../../../bin/em
		-cmp em.1 ../../../man/em.1

clean:
		-rm -f *.o *.old a.out em tmp.s $(INTS) compile

opr:
	make pr | opr

pr:
	@pr `pwd`/makefile
	@pr `pwd`/deffile `pwd`/$P?
