EMHOME = ../../..
MODULES = $(EMHOME)/modules
HDIR =	$(MODULES)/h
EMHDIR = $(EMHOME)/h
INCLUDES = -I$(HDIR) -I$(EMHDIR)
DEFINES = -DPRIVATE=static -DEXPORT=
CFLAGS = -O $(INCLUDES) $(DEFINES)
INSTALL = $(MODULES)/install
COMPARE = $(MODULES)/compare

TARGETS =	libread_emk.a\
		libread_emkV.a\
		libread_emeV.a\
		makecallsO.o\
		makecallsCE.o

ESRCFILES =	read_em.c\
		mkcalls.c\
		EM_vars.c

KSRCFILES =	read_em.c\
		mkcalls.c\
		EM_vars.c

SRCFILES =	em_comp.h\
		read_em.c\
		reade.c\
		readk.c \
		mkcalls.c\
		EM_vars.c

EV_OFILES =	read_emeV.o makecallsV.o EM_vars.o
KV_OFILES = 	read_emkV.o makecallsV.o EM_vars.o
K_OFILES =	read_emk.o makecalls.o EM_vars.o

all:		$(TARGETS)

install:	all
		$(INSTALL) h/em_comp.h
		$(INSTALL) lib/libread_emk.a
		$(INSTALL) lib/libread_emkV.a
		$(INSTALL) lib/libread_emeV.a
		$(INSTALL) man/read_em.3
		$(INSTALL) lib/makecallsO.o
		$(INSTALL) lib/makecallsCE.o

cmp:		all
		$(COMPARE) h/em_comp.h
		$(COMPARE) lib/libread_emk.a
		$(COMPARE) lib/libread_emkV.a
		$(COMPARE) lib/libread_emeV.a
		$(COMPARE) man/read_em.3
		$(COMPARE) lib/makecallsO.o
		$(COMPARE) lib/makecallsCE.o

pr:
		@pr Makefile m_C_mnem m_C_mnem_na argtype $(SRCFILES)

opr:
		make pr | opr

clean:
		rm -f *.o *.a C_mnem C_mnem_narg

libread_emk.a:	$(K_OFILES)
		ar r libread_emk.a $(K_OFILES)
		-sh -c 'ranlib libread_emk.a'

libread_emkV.a:	$(KV_OFILES)
		ar r libread_emkV.a $(KV_OFILES)
		-sh -c 'ranlib libread_emkV.a'

libread_emeV.a:	$(EV_OFILES)
		ar r libread_emeV.a $(EV_OFILES)
		-sh -c 'ranlib libread_emeV.a'

read_emk.o:	read_em.c em_comp.h readk.c
		$(CC) -c $(CFLAGS) -DCOMPACT read_em.c
		mv read_em.o read_emk.o

read_emkV.o:	read_em.c em_comp.h readk.c
		$(CC) -c $(CFLAGS) -DCOMPACT -DCHECKING read_em.c
		mv read_em.o read_emkV.o

read_emeV.o:	read_em.c em_comp.h reade.c
		$(CC) -c $(CFLAGS) -DCHECKING read_em.c
		mv read_em.o read_emeV.o

makecalls.o:	C_mnem C_mnem_narg em_comp.h mkcalls.c
		$(CC) -c $(CFLAGS) mkcalls.c
		mv mkcalls.o makecalls.o

makecallsV.o:	C_mnem C_mnem_narg em_comp.h mkcalls.c
		$(CC) -c $(CFLAGS) -DCHECKING mkcalls.c
		mv mkcalls.o makecallsV.o

makecallsO.o:	C_mnem C_mnem_narg em_comp.h mkcalls.c
		$(CC) -c -DPEEPHOLE $(CFLAGS) mkcalls.c
		mv mkcalls.o makecallsO.o

makecallsCE.o:	C_mnem C_mnem_narg em_comp.h mkcalls.c
		$(CC) -c -DCODE_EXPANDER $(CFLAGS) mkcalls.c
		mv mkcalls.o makecallsCE.o

C_mnem:		m_C_mnem argtype
		sh m_C_mnem > C_mnem

C_mnem_narg:	m_C_mnem_na argtype
		sh m_C_mnem_na > C_mnem_narg

lintlib:	C_mnem C_mnem_narg
		lint $(INCLUDES) $(DEFINES) -DCOMPACT -DCHECKING -Cread_emkV $(KSRCFILES)
		lint $(INCLUDES) $(DEFINES) -DCHECKING -Cread_emeV $(ESRCFILES)
		mv llib-lread_emeV.ln llib-lread_emkV.ln $(MODULES)/lib
