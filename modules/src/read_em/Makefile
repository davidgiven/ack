EMHOME = ../../..
MODULES = $(EMHOME)/modules
HDIR =	$(MODULES)/h
EMHDIR = $(EMHOME)/h
INCLUDES = -I$(HDIR) -I$(EMHDIR)
DEFINES = -DPRIVATE=static -DEXPORT=
CFLAGS = -O $(INCLUDES) $(DEFINES) $(COPT)
INSTALL = $(MODULES)/install
COMPARE = $(MODULES)/compare
AR = ar
SUF = o
LIBSUF = a

TARGETS =	libread_emk.$(LIBSUF)\
		libread_emkV.$(LIBSUF)\
		libread_emeV.$(LIBSUF) # makecallsO.$(SUF) makecallsCE.$(SUF)

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

EV_OFILES =	read_emeV.$(SUF) makecallsV.$(SUF) EM_vars.$(SUF)
KV_OFILES = 	read_emkV.$(SUF) makecallsV.$(SUF) EM_vars.$(SUF)
K_OFILES =	read_emk.$(SUF) makecalls.$(SUF) EM_vars.$(SUF)

.SUFFIXES: .$(SUF)
.c.$(SUF):
	$(CC) -c $(CFLAGS) $*.c

all:		$(TARGETS)

install:	all
		$(INSTALL) h/em_comp.h
		$(INSTALL) lib/libread_emk.$(LIBSUF)
		$(INSTALL) lib/libread_emkV.$(LIBSUF)
		$(INSTALL) lib/libread_emeV.$(LIBSUF)
		$(INSTALL) man/read_em.3
#		$(INSTALL) lib/makecallsO.$(SUF)
#		$(INSTALL) lib/makecallsCE.$(SUF)

cmp:		all
		-$(COMPARE) h/em_comp.h
		-$(COMPARE) lib/libread_emk.$(LIBSUF)
		-$(COMPARE) lib/libread_emkV.$(LIBSUF)
		-$(COMPARE) lib/libread_emeV.$(LIBSUF)
		-$(COMPARE) man/read_em.3
#		-$(COMPARE) lib/makecallsO.$(SUF)
#		-$(COMPARE) lib/makecallsCE.$(SUF)

pr:
		@pr Makefile m_C_mnem m_C_mnem_na argtype $(SRCFILES)

opr:
		make pr | opr

clean:
		rm -f *.$(SUF) *.$(LIBSUF) C_mnem C_mnem_narg

libread_emk.$(LIBSUF):	$(K_OFILES)
		$(AR) r libread_emk.$(LIBSUF) $(K_OFILES)
		-sh -c 'ranlib libread_emk.$(LIBSUF)'

libread_emkV.$(LIBSUF):	$(KV_OFILES)
		$(AR) r libread_emkV.$(LIBSUF) $(KV_OFILES)
		-sh -c 'ranlib libread_emkV.$(LIBSUF)'

libread_emeV.$(LIBSUF):	$(EV_OFILES)
		$(AR) r libread_emeV.$(LIBSUF) $(EV_OFILES)
		-sh -c 'ranlib libread_emeV.$(LIBSUF)'

read_emk.$(SUF):	read_em.c em_comp.h readk.c
		$(CC) -c $(CFLAGS) -DCOMPACT read_em.c
		mv read_em.$(SUF) read_emk.$(SUF)

read_emkV.$(SUF):	read_em.c em_comp.h readk.c
		$(CC) -c $(CFLAGS) -DCOMPACT -DCHECKING read_em.c
		mv read_em.$(SUF) read_emkV.$(SUF)

read_emeV.$(SUF):	read_em.c em_comp.h reade.c
		$(CC) -c $(CFLAGS) -DCHECKING read_em.c
		mv read_em.$(SUF) read_emeV.$(SUF)

makecalls.$(SUF):	C_mnem C_mnem_narg em_comp.h mkcalls.c
		$(CC) -c $(CFLAGS) mkcalls.c
		mv mkcalls.$(SUF) makecalls.$(SUF)

makecallsV.$(SUF):	C_mnem C_mnem_narg em_comp.h mkcalls.c
		$(CC) -c $(CFLAGS) -DCHECKING mkcalls.c
		mv mkcalls.$(SUF) makecallsV.$(SUF)

makecallsO.$(SUF):	C_mnem C_mnem_narg em_comp.h mkcalls.c
		$(CC) -c -DPEEPHOLE $(CFLAGS) mkcalls.c
		mv mkcalls.$(SUF) makecallsO.$(SUF)

makecallsCE.$(SUF):	C_mnem C_mnem_narg em_comp.h mkcalls.c
		$(CC) -c -DCODE_EXPANDER $(CFLAGS) mkcalls.c
		mv mkcalls.$(SUF) makecallsCE.$(SUF)

C_mnem:		m_C_mnem argtype
		sh m_C_mnem > C_mnem

C_mnem_narg:	m_C_mnem_na argtype
		sh m_C_mnem_na > C_mnem_narg

lintlib:	C_mnem C_mnem_narg
		lint $(INCLUDES) $(DEFINES) -DCOMPACT -DCHECKING -Cread_emkV $(KSRCFILES)
		lint $(INCLUDES) $(DEFINES) -DCHECKING -Cread_emeV $(ESRCFILES)
		mv llib-lread_emeV.ln llib-lread_emkV.ln $(MODULES)/lib
