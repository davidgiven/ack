# $Header$

#PARAMS		do not remove this line!

SRC_DIR = $(SRC_HOME)/modules/src/em_mes
MOD_DIR = $(TARGET_HOME)/modules
INCLUDES=-I$(TARGET_HOME)/h -I$(MOD_DIR)/h
CFLAGS = $(INCLUDES) $(COPTIONS)

SRC =	$(SRC_DIR)/C_ms_err.c \
	$(SRC_DIR)/C_ms_opt.c \
	$(SRC_DIR)/C_ms_emx.c \
	$(SRC_DIR)/C_ms_reg.c \
	$(SRC_DIR)/C_ms_src.c \
	$(SRC_DIR)/C_ms_flt.c \
	$(SRC_DIR)/C_ms_com.c \
	$(SRC_DIR)/C_ms_par.c \
	$(SRC_DIR)/C_ms_ego.c \
	$(SRC_DIR)/C_ms_gto.c \
	$(SRC_DIR)/C_ms_stb.c \
	$(SRC_DIR)/C_ms_std.c
OBJ =	C_ms_err.$(SUF) C_ms_opt.$(SUF) C_ms_emx.$(SUF) C_ms_reg.$(SUF) \
	C_ms_src.$(SUF) C_ms_flt.$(SUF) C_ms_com.$(SUF) C_ms_par.$(SUF) \
	C_ms_ego.$(SUF) C_ms_gto.$(SUF) C_ms_stb.$(SUF) C_ms_std.$(SUF)

all:		libem_mes.$(LIBSUF) libem_mesCE.$(LIBSUF) libem_mesO.$(LIBSUF) 

libem_mes.$(LIBSUF):	$(OBJ)
		rm -f libem_mes.$(LIBSUF)
		$(AR) r libem_mes.$(LIBSUF) $(OBJ)
		$(RANLIB) libem_mes.$(LIBSUF) 

libem_mesO.$(LIBSUF):
		rm -f *.$(SUF) libem_mesO.$(LIBSUF)
		$(CC) -c $(CFLAGS) -DPEEPHOLE $(SRC)
		$(AR) r libem_mesO.$(LIBSUF) $(OBJ)
		$(RANLIB) libem_mesO.$(LIBSUF)
		rm -f *.$(SUF)

libem_mesCE.$(LIBSUF):
		rm -f *.$(SUF) libem_mesCE.$(LIBSUF)
		$(CC) -c $(CFLAGS) -DCODE_EXPANDER $(SRC)
		$(AR) r libem_mesCE.$(LIBSUF) $(OBJ)
		$(RANLIB) libem_mesCE.$(LIBSUF)
		rm -f *.$(SUF)

install:	all
		-mkdir $(MOD_DIR)/lib
		cp libem_mes.$(LIBSUF) $(MOD_DIR)/lib/libem_mes.$(LIBSUF)
		cp libem_mesCE.$(LIBSUF) $(MOD_DIR)/lib/libem_mesCE.$(LIBSUF)
		cp libem_mesO.$(LIBSUF) $(MOD_DIR)/lib/libem_mesO.$(LIBSUF)
		$(RANLIB) $(MOD_DIR)/lib/libem_mes.$(LIBSUF)
		$(RANLIB) $(MOD_DIR)/lib/libem_mesCE.$(LIBSUF)
		$(RANLIB) $(MOD_DIR)/lib/libem_mesO.$(LIBSUF)
		if [ $(DO_MACHINE_INDEP) = y ] ; \
		then	mk_manpage $(SRC_DIR)/em_mes.3 $(TARGET_HOME) ; \
		fi

cmp:		all
		-cmp libem_mes.$(LIBSUF) $(MOD_DIR)/lib/libem_mes.$(LIBSUF)
		-cmp libem_mesCE.$(LIBSUF) $(MOD_DIR)/lib/libem_mesCE.$(LIBSUF)
		-cmp libem_mesO.$(LIBSUF) $(MOD_DIR)/lib/libem_mesO.$(LIBSUF)

pr:
		@pr $(SRC_DIR)/proto.make $(SRC)

opr:
		make pr | opr

clean:
		rm -f *.$(SUF) *.$(LIBSUF)

lintlib:
		$(MK_LINT_LIB) em_mes $(MOD_DIR)/lib $(INCLUDES) $(SRC)

C_ms_err.$(SUF):	$(SRC_DIR)/C_ms_err.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/C_ms_err.c

C_ms_opt.$(SUF):	$(SRC_DIR)/C_ms_opt.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/C_ms_opt.c

C_ms_emx.$(SUF):	$(SRC_DIR)/C_ms_emx.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/C_ms_emx.c

C_ms_reg.$(SUF):	$(SRC_DIR)/C_ms_reg.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/C_ms_reg.c

C_ms_src.$(SUF):	$(SRC_DIR)/C_ms_src.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/C_ms_src.c

C_ms_flt.$(SUF):	$(SRC_DIR)/C_ms_flt.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/C_ms_flt.c

C_ms_com.$(SUF):	$(SRC_DIR)/C_ms_com.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/C_ms_com.c

C_ms_par.$(SUF):	$(SRC_DIR)/C_ms_par.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/C_ms_par.c

C_ms_ego.$(SUF):	$(SRC_DIR)/C_ms_ego.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/C_ms_ego.c

C_ms_gto.$(SUF):	$(SRC_DIR)/C_ms_gto.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/C_ms_gto.c

C_ms_stb.$(SUF):	$(SRC_DIR)/C_ms_stb.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/C_ms_stb.c

C_ms_std.$(SUF):	$(SRC_DIR)/C_ms_std.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/C_ms_std.c
