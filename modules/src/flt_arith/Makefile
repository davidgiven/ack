# $Header$
EMHOME = ../../..
MODDIR=$(EMHOME)/modules
INSTALL = $(MODDIR)/install
COMPARE = $(MODDIR)/compare
INCLUDES = -I. -I$(MODDIR)/h
CFLAGS = -O $(INCLUDES) $(COPT)
AR = ar
SUF = o
LIBSUF = a

LIBFLT = libflt.$(LIBSUF)

SRC =	flt_ar2flt.c flt_div.c flt_flt2ar.c flt_modf.c flt_str2fl.c \
	flt_cmp.c flt_add.c b64_add.c flt_mul.c flt_nrm.c b64_sft.c \
	flt_umin.c flt_chk.c split.c ucmp.c

OBJ =	flt_ar2flt.$(SUF) flt_div.$(SUF) flt_flt2ar.$(SUF) flt_modf.$(SUF) \
	flt_str2fl.$(SUF) flt_cmp.$(SUF) flt_add.$(SUF) b64_add.$(SUF) \
	flt_mul.$(SUF) flt_nrm.$(SUF) b64_sft.$(SUF) flt_umin.$(SUF) \
	flt_chk.$(SUF) split.$(SUF) ucmp.$(SUF)

.SUFFIXES:	.$(SUF)
.c.$(SUF):
	$(CC) -c $(CFLAGS) $*.c

all:		$(LIBFLT)

test:		$(LIBFLT) test.c
		$(CC) -o tst test.c $(LIBFLT)
		./tst

$(LIBFLT):	$(OBJ)
		rm -f $(LIBFLT)
		$(AR) r $(LIBFLT) $(OBJ)
		-sh -c 'ranlib $(LIBFLT)'

install:	all
		$(INSTALL) lib/$(LIBFLT)
		$(INSTALL) h/flt_arith.h
		$(INSTALL) man/flt_arith.3

cmp:		all
		-$(COMPARE) lib/$(LIBFLT)
		-$(COMPARE) h/flt_arith.h
		-$(COMPARE) man/flt_arith.3

pr:
		@pr Makefile $(SRC)

opr:
		make pr | opr

clean:
		rm -f *.$(SUF) $(LIBFLT)

lintlib:
		lint $(INCLUDES) -Cflt $(SRC)
		mv llib-lflt.ln $(MODDIR)/lib

b64_add.$(SUF):	flt_misc.h flt_arith.h
flt_ar2flt.$(SUF):	flt_misc.h flt_arith.h
flt_div.$(SUF):	flt_misc.h flt_arith.h
flt_nrm.$(SUF):	flt_misc.h flt_arith.h
b64_sft.$(SUF):	flt_misc.h flt_arith.h
flt_chk.$(SUF):	flt_misc.h flt_arith.h
flt_flt2ar.$(SUF):	flt_misc.h flt_arith.h
flt_str2fl.$(SUF):	flt_misc.h flt_arith.h
flt_add.$(SUF):	flt_misc.h flt_arith.h
flt_cmp.$(SUF):	flt_misc.h flt_arith.h
flt_mul.$(SUF):	flt_misc.h flt_arith.h
flt_modf.$(SUF):	flt_misc.h flt_arith.h
flt_umin.$(SUF):	flt_misc.h
ucmp.$(SUF):	flt_misc.h flt_arith.h
split.$(SUF):	flt_misc.h flt_arith.h
