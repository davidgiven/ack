# $Id$

#PARAMS		do not remove this line!

SRC_DIR = $(SRC_HOME)/modules/src/read_em
MOD_DIR = $(TARGET_HOME)/modules
EM_TABLE = $(SRC_HOME)/etc/em_table
INCLUDES = -I$(TARGET_HOME)/h -I$(MOD_DIR)/h -I$(SRC_DIR) -I.
DEFINES = -DPRIVATE=static -DEXPORT= -DNDEBUG
CFLAGS = $(INCLUDES) $(DEFINES) $(COPTIONS)

TARGETS =	libread_emk.$(LIBSUF) \
		libread_emkV.$(LIBSUF) \
		libread_emeV.$(LIBSUF)

ESRCFILES =	$(SRC_DIR)/read_em.c\
		$(SRC_DIR)/mkcalls.c\
		$(SRC_DIR)/EM_vars.c

KSRCFILES =	$(SRC_DIR)/read_em.c\
		$(SRC_DIR)/mkcalls.c\
		$(SRC_DIR)/EM_vars.c

SRCFILES =	$(SRC_DIR)/em_comp.h\
		$(SRC_DIR)/read_em.c\
		$(SRC_DIR)/reade.c\
		$(SRC_DIR)/readk.c \
		$(SRC_DIR)/mkcalls.c\
		$(SRC_DIR)/EM_vars.c

EV_OFILES =	read_emeV.$(SUF) makecallsV.$(SUF) EM_vars.$(SUF)
KV_OFILES = 	read_emkV.$(SUF) makecallsV.$(SUF) EM_vars.$(SUF)
K_OFILES =	read_emk.$(SUF) makecalls.$(SUF) EM_vars.$(SUF)

all:		$(TARGETS)

install:	all
		-mkdir $(MOD_DIR)/lib
		-mkdir $(MOD_DIR)/h
		cp $(SRC_DIR)/em_comp.h $(MOD_DIR)/h/em_comp.h
		cp libread_emk.$(LIBSUF) $(MOD_DIR)/lib/libread_emk.$(LIBSUF)
		$(RANLIB) $(MOD_DIR)/lib/libread_emk.$(LIBSUF)
		cp libread_emkV.$(LIBSUF) $(MOD_DIR)/lib/libread_emkV.$(LIBSUF)
		$(RANLIB) $(MOD_DIR)/lib/libread_emkV.$(LIBSUF)
		cp libread_emeV.$(LIBSUF) $(MOD_DIR)/lib/libread_emeV.$(LIBSUF)
		$(RANLIB) $(MOD_DIR)/lib/libread_emeV.$(LIBSUF)
		if [ $(DO_MACHINE_INDEP) = y ] ; \
		then	mk_manpage $(SRC_DIR)/read_em.3 $(TARGET_HOME) ; \
		fi

cmp:		all
		-cmp $(SRC_DIR)/em_comp.h $(MOD_DIR)/h/em_comp.h
		-cmp libread_emk.$(LIBSUF) $(MOD_DIR)/lib/libread_emk.$(LIBSUF)
		-cmp libread_emkV.$(LIBSUF) $(MOD_DIR)/lib/libread_emkV.$(LIBSUF)
		-cmp libread_emeV.$(LIBSUF) $(MOD_DIR)/lib/libread_emeV.$(LIBSUF)

pr:
		@pr $(SRC_DIR)/proto.make $(SRC_DIR)/m_C_mnem $(SRC_DIR)/m_C_mnem_na $(SRC_DIR)/argtype $(SRCFILES)

opr:
		make pr | opr

clean:
		rm -f *.$(SUF) *.$(LIBSUF) C_mnem C_mnem_narg

libread_emk.$(LIBSUF):	$(K_OFILES)
		rm -f libread_emk.$(LIBSUF)
		$(AR) r libread_emk.$(LIBSUF) $(K_OFILES)
		$(RANLIB) libread_emk.$(LIBSUF)

libread_emkV.$(LIBSUF):	$(KV_OFILES)
		rm -f libread_emkV.$(LIBSUF)
		$(AR) r libread_emkV.$(LIBSUF) $(KV_OFILES)
		$(RANLIB) libread_emkV.$(LIBSUF)

libread_emeV.$(LIBSUF):	$(EV_OFILES)
		rm -f libread_emeV.$(LIBSUF)
		$(AR) r libread_emeV.$(LIBSUF) $(EV_OFILES)
		$(RANLIB) libread_emeV.$(LIBSUF)

read_emk.$(SUF):	$(SRC_DIR)/read_em.c $(SRC_DIR)/em_comp.h $(SRC_DIR)/readk.c
		$(CC) -c $(CFLAGS) -DCOMPACT $(SRC_DIR)/read_em.c
		mv read_em.$(SUF) read_emk.$(SUF)

read_emkV.$(SUF):	$(SRC_DIR)/read_em.c $(SRC_DIR)/em_comp.h $(SRC_DIR)/readk.c
		$(CC) -c $(CFLAGS) -DCOMPACT -DCHECKING $(SRC_DIR)/read_em.c
		mv read_em.$(SUF) read_emkV.$(SUF)

read_emeV.$(SUF):	$(SRC_DIR)/read_em.c $(SRC_DIR)/em_comp.h $(SRC_DIR)/reade.c
		$(CC) -c $(CFLAGS) -DCHECKING $(SRC_DIR)/read_em.c
		mv read_em.$(SUF) read_emeV.$(SUF)

makecalls.$(SUF):	C_mnem C_mnem_narg $(SRC_DIR)/em_comp.h $(SRC_DIR)/mkcalls.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/mkcalls.c
		mv mkcalls.$(SUF) makecalls.$(SUF)

makecallsV.$(SUF):	C_mnem C_mnem_narg $(SRC_DIR)/em_comp.h $(SRC_DIR)/mkcalls.c
		$(CC) -c $(CFLAGS) -DCHECKING $(SRC_DIR)/mkcalls.c
		mv mkcalls.$(SUF) makecallsV.$(SUF)

makecallsO.$(SUF):	C_mnem C_mnem_narg $(SRC_DIR)/em_comp.h $(SRC_DIR)/mkcalls.c
		$(CC) -c -DPEEPHOLE $(CFLAGS) $(SRC_DIR)/mkcalls.c
		mv mkcalls.$(SUF) makecallsO.$(SUF)

makecallsCE.$(SUF):	C_mnem C_mnem_narg $(SRC_DIR)/em_comp.h $(SRC_DIR)/mkcalls.c
		$(CC) -c -DCODE_EXPANDER $(CFLAGS) $(SRC_DIR)/mkcalls.c
		mv mkcalls.$(SUF) makecallsCE.$(SUF)

C_mnem:		$(SRC_DIR)/m_C_mnem argtype $(EM_TABLE)
		sh $(SRC_DIR)/m_C_mnem $(EM_TABLE) > C_mnem

C_mnem_narg:	$(SRC_DIR)/m_C_mnem_na argtype $(EM_TABLE)
		sh $(SRC_DIR)/m_C_mnem_na $(EM_TABLE) > C_mnem_narg

argtype:	$(SRC_DIR)/argtype
		cp $(SRC_DIR)/argtype argtype

lintlib:	C_mnem C_mnem_narg
		$(MK_LINT_LIB) read_emkV $(MOD_DIR)/lib $(INCLUDES) $(DEFINES) -DCOMPACT -DCHECKING $(KSRCFILES)
		$(MK_LINT_LIB) read_emeV $(MOD_DIR)/lib $(INCLUDES) $(DEFINES) -DCHECKING $(ESRCFILES)

EM_vars.$(SUF):	$(SRC_DIR)/EM_vars.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/EM_vars.c
