EMROOT=../../..
ACK=$(EMROOT)/bin/$(MACH)
A68S=$(EMROOT)/lib/em_a68s$(w)$(p)
A68INIT=$(EMROOT)/lib/em_a68s_init$(w)$(p)
PC=$(ACK) -.p -PR$(EMROOT)/lang/a68s/cpem/cpem 
PCFLAGS=-v -e -L
UTIL=../util
TAILOR=$(UTIL)/tailor
CHECKSEQ=$(UTIL)/checkseq
XREF=$(UTIL)/xref -i$(UTIL)/pascal.ign -p
TERRS=/dev/tty
TNOS=101 2 103 104 105 111 122 123 24 125 32 133 41 42 150 151 152 153 154 155 161 $(RECIPE)
SFILES=a68sdec.p a68sdum.p a68sin.p a68ssp.p a68spar.p a68scod.p
S1FILES=a68sdec.p a68s1int.p a68s1lx.p a68s1ce.p a68s1cg.p a68s1md.p a68s1s1.p a68s1s2.p a68s1pa.p
OTHFILES=cmpdum.p getaddr.e dec_main.p dec_main_s1.p Makefile

all:		a68init$(w)$(p) a68s$(w)$(p)

cmp:		a68init$(w)$(p) a68s$(w)$(p)
		-cmp a68init$(w)$(p) $(A68INIT)
		-cmp a68s$(w)$(p) $(A68S)

install:	a68init$(w)$(p) a68s$(w)$(p)
		rm -f $(A68S) $(A68INIT)
		cp a68init$(w)$(p) $(A68INIT)
		cp a68s$(w)$(p) $(A68S)

getaddr.o:	getaddr.e
		$(ACK) -c.o -DEM_WSIZE=$(w) -DEM_PSIZE=$(p) -v getaddr.e

pcalls.o:	pcalls.e
		$(ACK) -c.o -DEM_WSIZE=$(w) -DEM_PSIZE=$(p) -v pcalls.e

init1:		init1.out cmpdum
		init1.out /dev/null /dev/null init1lst /dev/null f1
		init1.out /dev/null Makefile init1lst /dev/null f2
		cmpdum f1 f2 init >>init1lst
		rm f1 f2
		mv init init1

init1.out:	a68sdum.p a68sin.p a68sdec0.h lx1.o getaddr.o pcalls.o
		(echo '#include "a68sdec0.h"';\
		 echo $(TNOS) 300 | $(TAILOR) a68sint.p $(TERRS); \
		 echo $(TNOS) 83 300 | $(TAILOR) a68sdum.p $(TERRS);\
		 echo $(TNOS) 81 83 184 300| $(TAILOR) a68sin.p $(TERRS); )\
		    >temp.p
		$(PC) $(PCFLAGS) -c.o temp.p
		$(PC) $(PCFLAGS) -o init1.out pcalls.o temp.o getaddr.o lx1.o
		rm temp.o

init2:		init1 init2.out cmpdum
		init2.out /dev/null /dev/null init2lst init1 f1
		init2.out /dev/null Makefile init2lst init1 f2
		cmpdum f1 f2 init >>init2lst
		rm f1 f2
		mv init init2

init2.out:	a68sdum.p a68sin.p a68sdec4.h lx1.o lx4.o getaddr.o pcalls.o
		(echo '#include "a68sdec4.h"';\
		 echo $(TNOS) 84 300 | $(TAILOR) a68sint.p $(TERRS); \
		 echo $(TNOS) 83 300 | $(TAILOR) a68sdum.p $(TERRS);\
		 echo $(TNOS) 181 83 84 300| $(TAILOR) a68sin.p $(TERRS); )\
		    >temp.p
		$(PC) $(PCFLAGS) -c.o temp.p
		$(PC) $(PCFLAGS) -o init2.out pcalls.o temp.o lx4.o getaddr.o lx1.o 
		rm temp.o

init3:		init2 init3.out cmpdum syntax
		init3.out syntax /dev/null init3lst init2 f1
		init3.out syntax Makefile init3lst init2 f2
		cmpdum f1 f2 init >>init3lst
		rm f1 f2
		mv init init3

init3.out:	a68sdum.p a68spar.p a68sdec2.h lx1.o lx2.o getaddr.o pcalls.o
		(echo '#include "a68sdec2.h"';\
		 echo $(TNOS) 82 300 | $(TAILOR) a68sint.p $(TERRS); \
		 echo $(TNOS) 82 300 | $(TAILOR) a68sdum.p $(TERRS);\
		 echo $(TNOS) 82 300 | $(TAILOR) a68spar.p $(TERRS); )\
		    >temp.p
		$(PC) $(PCFLAGS) -c.o temp.p
		$(PC) $(PCFLAGS) -o init3.out pcalls.o temp.o lx2.o getaddr.o lx1.o
		rm temp.o

init4:		init3 init4.out cmpdum
		init4.out /dev/null /dev/null init4lst init3 f1
		init4.out /dev/null Makefile init4lst init3 f2
		cmpdum f1 f2 init >>init4lst
		rm f1 f2
		mv init init4

init4.out:	a68sdum.p a68ssp.p a68sdec4.h lx1.o lx4.o getaddr.o pcalls.o
		(echo '#include "a68sdec4.h"';\
		 echo $(TNOS) 84 300 | $(TAILOR) a68sint.p $(TERRS); \
		 echo $(TNOS) 85 300 | $(TAILOR) a68sdum.p $(TERRS);\
		 echo $(TNOS) 85 300 | $(TAILOR) a68ssp.p $(TERRS); )\
		    >temp.p
		$(PC) $(PCFLAGS) -c.o temp.p
		$(PC) $(PCFLAGS) -o init4.out pcalls.o temp.o lx4.o getaddr.o lx1.o
		rm temp.o

a68init:	a68init$(w)$(p)

a68init$(w)$(p):	init4 init5.out cmpdum
		init5.out /dev/null /dev/null init5lst init4 f1
		init5.out /dev/null Makefile init5lst init4 f2
		cmpdum f1 f2 init >>init5lst
		rm f1 f2
		mv init a68init$(w)$(p)

init5.out:	a68sdum.p a68scod.p a68sdec5.h lx1.o getaddr.o pcalls.o
		(echo '#include "a68sdec5.h"';\
		 echo $(TNOS) 300 | $(TAILOR) a68sint.p $(TERRS); \
		 echo $(TNOS) 86 300 | $(TAILOR) a68sdum.p $(TERRS);\
		 echo $(TNOS) 86 300 | $(TAILOR) a68scod.p $(TERRS); )\
		    >temp.p
		$(PC) $(PCFLAGS) -c.o temp.p
		$(PC) $(PCFLAGS) -o init5.out pcalls.o temp.o getaddr.o lx1.o
		rm temp.[op]

cmpdum:		check$(w)$(p) cmpdum.p
		echo $(TNOS) 300 | $(TAILOR) cmpdum.p $(TERRS) >temp.p
		$(PC) $(PCFLAGS) -o cmpdum temp.p

a68sdec0.h:	check$(w)$(p) a68sdec.p
		echo $(TNOS) 70 171 172 73 174 175 176 177 178 300\
		| $(TAILOR) a68sdec.p $(TERRS) >a68sdec0.h

a68sdec2.h:	check$(w)$(p) a68sdec.p
		echo $(TNOS) 70 171 72 73 174 175 176 177 178 300\
		| $(TAILOR) a68sdec.p $(TERRS) >a68sdec2.h

a68sdec4.h:	check$(w)$(p) a68sdec.p
		echo $(TNOS) 70 171 172 73 74 75 176 177 178 300\
		| $(TAILOR) a68sdec.p $(TERRS) >a68sdec4.h

a68sdec5.h:	check$(w)$(p) a68sdec.p
		echo $(TNOS) 70 171 172 173 174 75 76 177 78 300\
		| $(TAILOR) a68sdec.p $(TERRS) >a68sdec5.h

a68sdec6.h:	check$(w)$(p) a68sdec.p
		echo $(TNOS) 70 171 172 73 174 175 76 77 78 300\
		| $(TAILOR) a68sdec.p $(TERRS) >a68sdec6.h

lx1.o:		check$(w)$(p) a68s1lx.p a68sdec.p dec_main.p
		(echo $(TNOS) 70 71 172 73 174 175 176 177 178 300\
		 | $(TAILOR) a68sdec.p $(TERRS);\
		 echo $(TNOS) 81 282 284 285 286 300\
		 | $(TAILOR) a68s1lx.p $(TERRS);\
		 cat dec_main.p ) |\
		cat >temp.p
		$(PC) $(PCFLAGS) -c.o temp.p 
		mv temp.o lx1.o

lx1s1.o:	check$(w)$(p) a68s1lx.p a68sdec.p dec_main_s1.p
		(echo $(TNOS) 70 71 172 73 174 175 176 177 178 300\
		 | $(TAILOR) a68sdec.p $(TERRS);\
		 echo $(TNOS) 81 282 284 285 286 300\
		 | $(TAILOR) a68s1lx.p $(TERRS);\
		 cat dec_main_s1.p ) |\
		cat >temps.p
		$(PC) $(PCFLAGS) -c.o temps.p 
		mv temps.o lx1s1.o

lx2.o:		check$(w)$(p) a68s1lx.p a68sdec.p
		(echo $(TNOS) 70 171 72 73 174 175 176 177 178 300\
		 | $(TAILOR) a68sdec.p $(TERRS);\
		 echo $(TNOS) 300 | $(TAILOR) a68sint.p $(TERRS); \
		 echo $(TNOS) 281 82 284 285 286 300\
		 | $(TAILOR) a68s1lx.p $(TERRS) )\
			> temp.p
		$(PC) $(PCFLAGS) -c.o temp.p
		mv temp.o lx2.o

lx4.o:		check$(w)$(p) a68s1lx.p a68sdec.p
		(echo $(TNOS) 70 171 172 73 74 75 176 177 178 300\
		 | $(TAILOR) a68sdec.p $(TERRS);\
		 echo $(TNOS) 300 | $(TAILOR) a68sint.p $(TERRS); \
		 echo $(TNOS) 281 282 84 285 286 300\
		 | $(TAILOR) a68s1lx.p $(TERRS) )\
			> temp.p
		$(PC) $(PCFLAGS) -c.o temp.p
		mv temp.o lx4.o

a68s1ce.o:	a68s1ce.p a68sdec6.h a68s1int.p
		(echo '#include "a68sdec6.h"'; \
		 echo $(TNOS) 182 183 184 185 186 87 300 | $(TAILOR) a68s1int.p $(TERRS); \
		 echo $(TNOS) 87 300 | $(TAILOR) a68s1ce.p $(TERRS) ) >temps.p
		$(PC) $(PCFLAGS) -c.o temps.p
		mv temps.o a68s1ce.o

a68s1cg.o:	a68s1cg.p a68sdec6.h a68s1int.p
		(echo '#include "a68sdec6.h"'; \
		 echo $(TNOS) 182 183 184 185 86 187 300 | $(TAILOR) a68s1int.p $(TERRS); \
		 echo $(TNOS) 86 300 | $(TAILOR) a68s1cg.p $(TERRS) ) >temps.p
		$(PC) $(PCFLAGS) -c.o temps.p
		mv temps.o a68s1cg.o

a68s1md.o:	a68s1md.p a68sdec6.h a68s1int.p
		(echo '#include "a68sdec6.h"'; \
		 echo $(TNOS) 182 183 84 185 186 187 300 | $(TAILOR) a68s1int.p $(TERRS); \
		 echo $(TNOS) 84 300 | $(TAILOR) a68s1md.p $(TERRS) ) >temps.p
		$(PC) $(PCFLAGS) -c.o temps.p
		mv temps.o a68s1md.o

a68s1s1.o:	a68s1s1.p a68sdec4.h a68s1int.p
		(echo '#include "a68sdec4.h"'; \
		 echo $(TNOS) 182 183 184 85 186 187 300 | $(TAILOR) a68s1int.p $(TERRS); \
		 echo $(TNOS) 85 300 | $(TAILOR) a68s1s1.p $(TERRS) ) >temps.p
		$(PC) $(PCFLAGS) -t -c.o temps.p
		mv temps.o a68s1s1.o

a68s1s2.o:	a68s1s2.p a68sdec4.h a68s1int.p
		(echo '#include "a68sdec4.h"'; \
		 echo $(TNOS) 182 83 184 185 186 187 300 | $(TAILOR) a68s1int.p $(TERRS); \
		 echo $(TNOS) 83 300 | $(TAILOR) a68s1s2.p $(TERRS) ) >temps.p
		$(PC) $(PCFLAGS) -c.o temps.p
		mv temps.o a68s1s2.o

a68s1pa.o:	a68s1pa.p a68sdec2.h a68s1int.p
		(echo '#include "a68sdec2.h"'; \
		 echo $(TNOS) 82 183 184 185 186 187 300 | $(TAILOR) a68s1int.p $(TERRS); \
		 echo $(TNOS) 82 300 | $(TAILOR) a68s1pa.p $(TERRS) ) >temps.p
		$(PC) $(PCFLAGS) -c.o temps.p
		mv temps.o a68s1pa.o

a68s:		a68s$(w)$(p)

a68s$(w)$(p):	lx1s1.o lx2.o lx4.o a68s1ce.o a68s1cg.o a68s1md.o a68s1s1.o a68s1s2.o a68s1pa.o getaddr.o pcalls.o
		$(PC) $(PCFLAGS) -o a68s$(w)$(p) pcalls.o lx2.o lx4.o a68s1*.o getaddr.o lx1s1.o 
		rm temps.[pikms]

check$(w)$(p):	
		/bin/make clean
		echo >> check$(w)$(p)

checkseq:
		$(CHECKSEQ) $(SFILES) $(S1FILES) syntax

prs:
		pr $(SFILES)

xrefs:
		(/bin/make prs; \
		 for II in $(SFILES); do echo 1000 | $(TAILOR) $$II $(TERRS); done \
		 	| $(XREF) | pr -h a68init.xref \
		) | opr

pr1:
		pr $(S1FILES)

xref1:
		(/bin/make pr1; \
		 for II in $(S1FILES); do echo 1000 | $(TAILOR) $$II $(TERRS); done \
		 	| $(XREF) | pr -h a68s1.xref \
		) | opr

clean:		
		-rm *.[ho] *.out check?? init* cmpdum


