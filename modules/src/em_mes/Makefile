EMHOME = ../../..
HDIR = $(EMHOME)/modules/h
INSTALL = $(EMHOME)/modules/install
COMPARE = $(EMHOME)/modules/compare
INCLUDES=-I$(EMHOME)/h -I$(HDIR)
CFLAGS = $(INCLUDES) -O $(COPT)
AR = ar
SUF = o
LIBSUF = a

SRC =	C_ms_err.c C_ms_opt.c C_ms_emx.c C_ms_reg.c C_ms_src.c\
	C_ms_flt.c C_ms_com.c C_ms_par.c C_ms_ego.c C_ms_gto.c
OBJ =	C_ms_err.$(SUF) C_ms_opt.$(SUF) C_ms_emx.$(SUF) C_ms_reg.$(SUF) C_ms_src.$(SUF)\
	C_ms_flt.$(SUF) C_ms_com.$(SUF) C_ms_par.$(SUF) C_ms_ego.$(SUF) C_ms_gto.$(SUF)

.SUFFIXES: .$(SUF)
.c.$(SUF):
	$(CC) -c $(CFLAGS) $*.c

all:		libem_mes.$(LIBSUF) # libem_mesCE.$(LIBSUF) libem_mesO.$(LIBSUF) 

libem_mes.$(LIBSUF):	$(OBJ)
		rm -f libem_mes.$(LIBSUF)
		$(AR) cr libem_mes.$(LIBSUF) $(OBJ)
		-sh -c 'ranlib libem_mes.$(LIBSUF)'

libem_mesO.$(LIBSUF):
		rm -f *.$(SUF) libem_mesO.$(LIBSUF)
		$(CC) -c $(CFLAGS) -DPEEPHOLE $(SRC)
		$(AR) cr libem_mesO.$(LIBSUF) $(OBJ)
		-sh -c 'ranlib libem_mesO.$(LIBSUF)'
		rm -f *.$(SUF)

libem_mesCE.$(LIBSUF):
		rm -f *.$(SUF) libem_mesCE.$(LIBSUF)
		$(CC) -c $(CFLAGS) -DCODE_EXPANDER $(SRC)
		$(AR) cr libem_mesCE.$(LIBSUF) $(OBJ)
		-sh -c 'ranlib libem_mesCE.$(LIBSUF)'
		rm -f *.$(SUF)

install:	all
		$(INSTALL) lib/libem_mes.$(LIBSUF)
#		$(INSTALL) lib/libem_mesO.$(LIBSUF)
		$(INSTALL) man/em_mes.3

cmp:		all
		-$(COMPARE) lib/libem_mes.$(LIBSUF)
#		-$(COMPARE) lib/libem_mesO.$(LIBSUF)
		-$(COMPARE) man/em_mes.3

pr:
		@pr Makefile $(SRC)

opr:
		make pr | opr

clean:
		rm -f *.$(SUF) *.$(LIBSUF)

lintlib:
		lint $(INCLUDES) -Cem_mes $(SRC)
		mv llib-lem_mes.ln $(EMHOME)/modules/lib
