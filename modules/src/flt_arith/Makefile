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

SRC =	b64_add.c flt_ar2flt.c flt_div.c flt_nrm.c b64_sft.c flt_chk.c \
	flt_flt2ar.c flt_str2fl.c flt_add.c flt_cmp.c flt_mul.c ucmp.c \
	flt_modf.c flt_umin.c
OBJ =	b64_add.$(SUF) flt_ar2flt.$(SUF) flt_div.$(SUF) flt_nrm.$(SUF) \
	b64_sft.$(SUF) flt_chk.$(SUF) flt_flt2ar.$(SUF) flt_str2fl.$(SUF) \
	flt_add.$(SUF) flt_cmp.$(SUF) flt_mul.$(SUF) ucmp.$(SUF) \
	flt_modf.$(SUF) flt_umin.$(SUF)

.SUFFIXES:	.$(SUF)
.c.$(SUF):
	$(CC) -c $(CFLAGS) $*.c

all:		$(LIBFLT)

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

b64_add.$(SUF):	misc.h flt_arith.h
flt_ar2flt.$(SUF):	misc.h flt_arith.h
flt_div.$(SUF):	misc.h flt_arith.h
flt_nrm.$(SUF):	misc.h flt_arith.h
b64_sft.$(SUF):	misc.h flt_arith.h
flt_chk.$(SUF):	misc.h flt_arith.h
flt_flt2ar.$(SUF):	misc.h flt_arith.h
flt_str2fl.$(SUF):	misc.h flt_arith.h
flt_add.$(SUF):	misc.h flt_arith.h
flt_cmp.$(SUF):	misc.h flt_arith.h
flt_mul.$(SUF):	misc.h flt_arith.h
flt_modf.$(SUF):	misc.h flt_arith.h
flt_umin.$(SUF):	misc.h
ucmp.$(SUF):	misc.h flt_arith.h
