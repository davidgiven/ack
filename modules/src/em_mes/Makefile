EMHOME = ../../..
HDIR = $(EMHOME)/modules/h
INSTALL = $(EMHOME)/modules/install
COMPARE = $(EMHOME)/modules/compare
SRC =	C_ms_err.c C_ms_opt.c C_ms_emx.c C_ms_reg.c C_ms_src.c\
	C_ms_flt.c C_ms_com.c C_ms_par.c C_ms_ego.c C_ms_gto.c
OBJ =	C_ms_err.o C_ms_opt.o C_ms_emx.o C_ms_reg.o C_ms_src.o\
	C_ms_flt.o C_ms_com.o C_ms_par.o C_ms_ego.o C_ms_gto.o
INCLUDES=-I$(EMHOME)/h -I$(HDIR)
CFLAGS = $(INCLUDES) -O
AR = ar

all:		libem_mes.a # libem_mesCE.a libem_mesO.a 

libem_mes.a:	$(OBJ)
		rm -f libem_mes.a
		$(AR) cr libem_mes.a $(OBJ)
		-sh -c 'ranlib libem_mes.a'

libem_mesO.a:
		rm -f *.o libem_mesO.a
		$(CC) -c $(CFLAGS) -DPEEPHOLE $(SRC)
		$(AR) cr libem_mesO.a $(OBJ)
		-sh -c 'ranlib libem_mesO.a'
		rm -f *.o

libem_mesCE.a:
		rm -f *.o libem_mesCE.a
		$(CC) -c $(CFLAGS) -DCODE_EXPANDER $(SRC)
		$(AR) cr libem_mesCE.a $(OBJ)
		-sh -c 'ranlib libem_mesCE.a'
		rm -f *.o

install:	all
		$(INSTALL) lib/libem_mes.a
#		$(INSTALL) lib/libem_mesO.a
		$(INSTALL) man/em_mes.3

cmp:		all
		$(COMPARE) lib/libem_mes.a
#		$(COMPARE) lib/libem_mesO.a
		$(COMPARE) man/em_mes.3

pr:
		@pr Makefile $(SRC)

opr:
		make pr | opr

clean:
		rm -f *.[oa]

lintlib:
		lint $(INCLUDES) -Cem_mes $(SRC)
		mv llib-lem_mes.ln $(EMHOME)/modules/lib
