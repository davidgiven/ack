# $Id$

#PARAMS		do not remove this line!

SRC_DIR = $(SRC_HOME)/modules/src/flt_arith
MOD_DIR = $(TARGET_HOME)/modules
INCLUDES = -I$(SRC_DIR) -I$(MOD_DIR)/h
CFLAGS = $(INCLUDES) $(COPTIONS)
LDFLAGS = $(LDOPTIONS)

LIBFLT = libflt.$(LIBSUF)

SRC =	$(SRC_DIR)/flt_ar2flt.c \
	$(SRC_DIR)/flt_div.c \
	$(SRC_DIR)/flt_flt2ar.c  \
	$(SRC_DIR)/flt_modf.c  \
	$(SRC_DIR)/flt_str2fl.c \
	$(SRC_DIR)/flt_cmp.c  \
	$(SRC_DIR)/flt_add.c  \
	$(SRC_DIR)/b64_add.c  \
	$(SRC_DIR)/flt_mul.c  \
	$(SRC_DIR)/flt_nrm.c  \
	$(SRC_DIR)/b64_sft.c \
	$(SRC_DIR)/flt_umin.c  \
	$(SRC_DIR)/flt_chk.c  \
	$(SRC_DIR)/split.c  \
	$(SRC_DIR)/ucmp.c

OBJ =	flt_ar2flt.$(SUF) flt_div.$(SUF) flt_flt2ar.$(SUF) flt_modf.$(SUF) \
	flt_str2fl.$(SUF) flt_cmp.$(SUF) flt_add.$(SUF) b64_add.$(SUF) \
	flt_mul.$(SUF) flt_nrm.$(SUF) b64_sft.$(SUF) flt_umin.$(SUF) \
	flt_chk.$(SUF) split.$(SUF) ucmp.$(SUF)

all:		$(LIBFLT)

test:		$(LIBFLT) test.$(SUF)
		$(CC) $(LDFLAGS) -o tst test.$(SUF) $(LIBFLT)
		./tst

$(LIBFLT):	$(OBJ)
		rm -f $(LIBFLT)
		$(AR) r $(LIBFLT) $(OBJ)
		$(RANLIB) $(LIBFLT)

install:	all
		-mkdir $(MOD_DIR)/lib
		-mkdir $(MOD_DIR)/h
		cp $(LIBFLT) $(MOD_DIR)/lib/$(LIBFLT)
		$(RANLIB) $(MOD_DIR)/lib/$(LIBFLT)
		cp $(SRC_DIR)/flt_arith.h $(MOD_DIR)/h/flt_arith.h
		if [ $(DO_MACHINE_INDEP) = y ] ; \
		then	mk_manpage $(SRC_DIR)/flt_arith.3 $(TARGET_HOME) ; \
		fi

cmp:		all
		-cmp $(LIBFLT) $(MOD_DIR)/lib/$(LIBFLT)
		-cmp $(SRC_DIR)/flt_arith.h $(MOD_DIR)/h/flt_arith.h

pr:
		@pr $(SRC_DIR)/proto.make $(SRC)

opr:
		make pr | opr

clean:
		rm -f *.$(SUF) $(LIBFLT) tst

lintlib:
		$(MK_LINT_LIB) flt $(MOD_DIR)/lib $(INCLUDES) $(SRC)

b64_add.$(SUF):	$(SRC_DIR)/flt_misc.h $(SRC_DIR)/flt_arith.h $(SRC_DIR)/b64_add.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/b64_add.c

flt_ar2flt.$(SUF):	$(SRC_DIR)/flt_misc.h $(SRC_DIR)/flt_arith.h $(SRC_DIR)/flt_ar2flt.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/flt_ar2flt.c

flt_div.$(SUF):	$(SRC_DIR)/flt_misc.h $(SRC_DIR)/flt_arith.h $(SRC_DIR)/flt_div.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/flt_div.c

flt_nrm.$(SUF):	$(SRC_DIR)/flt_misc.h $(SRC_DIR)/flt_arith.h $(SRC_DIR)/flt_nrm.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/flt_nrm.c

b64_sft.$(SUF):	$(SRC_DIR)/flt_misc.h $(SRC_DIR)/flt_arith.h $(SRC_DIR)/b64_sft.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/b64_sft.c

flt_chk.$(SUF):	$(SRC_DIR)/flt_misc.h $(SRC_DIR)/flt_arith.h $(SRC_DIR)/flt_chk.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/flt_chk.c

flt_flt2ar.$(SUF):	$(SRC_DIR)/flt_misc.h $(SRC_DIR)/flt_arith.h $(SRC_DIR)/flt_flt2ar.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/flt_flt2ar.c

flt_str2fl.$(SUF):	$(SRC_DIR)/flt_misc.h $(SRC_DIR)/flt_arith.h $(SRC_DIR)/flt_str2fl.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/flt_str2fl.c

flt_add.$(SUF):	$(SRC_DIR)/flt_misc.h $(SRC_DIR)/flt_arith.h $(SRC_DIR)/flt_add.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/flt_add.c

flt_cmp.$(SUF):	$(SRC_DIR)/flt_misc.h $(SRC_DIR)/flt_arith.h $(SRC_DIR)/flt_cmp.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/flt_cmp.c

flt_mul.$(SUF):	$(SRC_DIR)/flt_misc.h $(SRC_DIR)/flt_arith.h $(SRC_DIR)/flt_mul.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/flt_mul.c

flt_modf.$(SUF):	$(SRC_DIR)/flt_misc.h $(SRC_DIR)/flt_arith.h $(SRC_DIR)/flt_modf.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/flt_modf.c

flt_umin.$(SUF):	$(SRC_DIR)/flt_misc.h $(SRC_DIR)/flt_umin.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/flt_umin.c

ucmp.$(SUF):	$(SRC_DIR)/flt_misc.h $(SRC_DIR)/flt_arith.h $(SRC_DIR)/ucmp.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/ucmp.c

split.$(SUF):	$(SRC_DIR)/flt_misc.h $(SRC_DIR)/flt_arith.h $(SRC_DIR)/split.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/split.c

test.$(SUF):	$(SRC_DIR)/flt_arith.h $(SRC_DIR)/test.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/test.c
