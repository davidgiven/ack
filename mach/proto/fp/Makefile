EMHOME=../../..
SUF=s
MACH=m68k4
ASAR=arch
CFLAGS=
#	must use -r option of make so that default rules
#	are not loaded
#
#	$Header$
#
EMFLAGS= -L -LIB -I. -O $(CFLAGS)
#	AS=ack -m$(MACH) -c.$(SUF)
#	CC=ack -m$(MACH) -c.s
#	CCFLAGS=$(EMFLAGS)
CDIR=$(EMHOME)/mach/proto/fp

LIST =		cff4.$(SUF) cff8.$(SUF)\
		cfu.$(SUF)\
		cmf4.$(SUF) cmf8.$(SUF)\
		cuf4.$(SUF) cuf8.$(SUF)\
		dvf4.$(SUF) dvf8.$(SUF)\
		fef4.$(SUF) fef8.$(SUF)\
		fif4.$(SUF) fif8.$(SUF)\
		cfi.$(SUF)\
		cif4.$(SUF) cif8.$(SUF)\
		mlf4.$(SUF) mlf8.$(SUF)\
		ngf4.$(SUF) ngf8.$(SUF)\
		sbf4.$(SUF) sbf8.$(SUF)\
		adf4.$(SUF) adf8.$(SUF)\
		zrf4.$(SUF) zrf8.$(SUF)\
		extend.$(SUF) compact.$(SUF)\
		add_ext.$(SUF) div_ext.$(SUF) mul_ext.$(SUF) nrm_ext.$(SUF)\
		sft_ext.$(SUF) sub_ext.$(SUF) zrf_ext.$(SUF)\
		adder.$(SUF) shifter.$(SUF) fptrp.$(SUF)
SLIST =		cff4.s cff8.s\
		cfu.s\
		cmf4.s cmf8.s\
		cuf4.s cuf8.s\
		dvf4.s dvf8.s\
		fef4.s fef8.s\
		fif4.s fif8.s\
		cfi.s\
		cif4.s cif8.s\
		mlf4.s mlf8.s\
		ngf4.s ngf8.s\
		sbf4.s sbf8.s\
		adf4.s adf8.s\
		zrf4.s zrf8.s\
		extend.s compact.s\
		add_ext.s div_ext.s mul_ext.s nrm_ext.s\
		sft_ext.s sub_ext.s zrf_ext.s\
		adder.s shifter.s fptrp.s

SRC =		FP_bias.h FP_shift.h FP_trap.h FP_types.h adder.h get_put.h\
		cff4.c cff8.c\
		cfu.c\
		cmf4.c cmf8.c\
		cuf4.c cuf8.c\
		dvf4.c dvf8.c\
		fef4.c fef8.c\
		fif4.c fif8.c\
		cfi.c\
		cif4.c cif8.c\
		mlf4.c mlf8.c\
		ngf4.c ngf8.c\
		sbf4.c sbf8.c\
		adf4.c adf8.c\
		zrf4.c zrf8.c\
		extend.c compact.c\
		add_ext.c div_ext.c mul_ext.c nrm_ext.c\
		sft_ext.c sub_ext.c zrf_ext.c\
		adder.c shifter.c fptrp.e

all:		FP_$(MACH).a

install:	tail_fp

tail_fp:	FP_$(MACH).a
	../../install FP_$(MACH).a tail_fp

clean:
	rm -f $(LIST) FP_$(MACH).a
	rm -f $(SLIST)

opr:
	make pr | opr

pr:
	@pr Makefile FP.script $(SRC)

FP_$(MACH).a:	$(LIST)
	$(ASAR) rv $@ $?

fptrp.$(SUF): $(CDIR)/fptrp.e
	ack -m$(MACH) -L -LIB -c $(CDIR)/fptrp.e

extend.$(SUF) compact.$(SUF): byte_order.h $(CDIR)/get_put.h

cff4.$(SUF):	$(CDIR)/cff4.c
	ack -c.s -m$(MACH) $(EMFLAGS) $(CDIR)/cff4.c
	ed - cff4.s <$(CDIR)/FP.script 
	ack -c -m$(MACH) $(EMFLAGS) cff4.s

cff8.$(SUF):	$(CDIR)/cff8.c
	ack -c.s -m$(MACH) $(EMFLAGS) $(CDIR)/cff8.c
	ed - cff8.s <$(CDIR)/FP.script 
	ack -c -m$(MACH) $(EMFLAGS) cff8.s

cfu.$(SUF):	$(CDIR)/cfu.c
	ack -c.s -m$(MACH) $(EMFLAGS) $(CDIR)/cfu.c
	ed - cfu.s <$(CDIR)/FP.script 
	ack -c -m$(MACH) $(EMFLAGS) cfu.s

cmf4.$(SUF):	$(CDIR)/cmf4.c
	ack -c.s -m$(MACH) $(EMFLAGS) $(CDIR)/cmf4.c
	ed - cmf4.s <$(CDIR)/FP.script 
	ack -c -m$(MACH) $(EMFLAGS) cmf4.s

cmf8.$(SUF):	$(CDIR)/cmf8.c
	ack -c.s -m$(MACH) $(EMFLAGS) $(CDIR)/cmf8.c
	ed - cmf8.s <$(CDIR)/FP.script 
	ack -c -m$(MACH) $(EMFLAGS) cmf8.s

cuf4.$(SUF):	$(CDIR)/cuf4.c
	ack -c.s -m$(MACH) $(EMFLAGS) $(CDIR)/cuf4.c
	ed - cuf4.s <$(CDIR)/FP.script 
	ack -c -m$(MACH) $(EMFLAGS) cuf4.s

cuf8.$(SUF):	$(CDIR)/cuf8.c
	ack -c.s -m$(MACH) $(EMFLAGS) $(CDIR)/cuf8.c
	ed - cuf8.s <$(CDIR)/FP.script 
	ack -c -m$(MACH) $(EMFLAGS) cuf8.s

dvf4.$(SUF):	$(CDIR)/dvf4.c
	ack -c.s -m$(MACH) $(EMFLAGS) $(CDIR)/dvf4.c
	ed - dvf4.s <$(CDIR)/FP.script 
	ack -c -m$(MACH) $(EMFLAGS) dvf4.s

dvf8.$(SUF):	$(CDIR)/dvf8.c
	ack -c.s -m$(MACH) $(EMFLAGS) $(CDIR)/dvf8.c
	ed - dvf8.s <$(CDIR)/FP.script 
	ack -c -m$(MACH) $(EMFLAGS) dvf8.s

fef4.$(SUF):	$(CDIR)/fef4.c
	ack -c.s -m$(MACH) $(EMFLAGS) $(CDIR)/fef4.c
	ed - fef4.s <$(CDIR)/FP.script 
	ack -c -m$(MACH) $(EMFLAGS) fef4.s

fef8.$(SUF):	$(CDIR)/fef8.c
	ack -c.s -m$(MACH) $(EMFLAGS) $(CDIR)/fef8.c
	ed - fef8.s <$(CDIR)/FP.script 
	ack -c -m$(MACH) $(EMFLAGS) fef8.s

fif4.$(SUF):	$(CDIR)/fif4.c
	ack -c.s -m$(MACH) $(EMFLAGS) $(CDIR)/fif4.c
	ed - fif4.s <$(CDIR)/FP.script 
	ack -c -m$(MACH) $(EMFLAGS) fif4.s

fif8.$(SUF):	$(CDIR)/fif8.c
	ack -c.s -m$(MACH) $(EMFLAGS) $(CDIR)/fif8.c
	ed - fif8.s <$(CDIR)/FP.script 
	ack -c -m$(MACH) $(EMFLAGS) fif8.s

cfi.$(SUF):	$(CDIR)/cfi.c
	ack -c.s -m$(MACH) $(EMFLAGS) $(CDIR)/cfi.c
	ed - cfi.s <$(CDIR)/FP.script 
	ack -c -m$(MACH) $(EMFLAGS) cfi.s

cif4.$(SUF):	$(CDIR)/cif4.c
	ack -c.s -m$(MACH) $(EMFLAGS) $(CDIR)/cif4.c
	ed - cif4.s <$(CDIR)/FP.script 
	ack -c -m$(MACH) $(EMFLAGS) cif4.s

cif8.$(SUF):	$(CDIR)/cif8.c
	ack -c.s -m$(MACH) $(EMFLAGS) $(CDIR)/cif8.c
	ed - cif8.s <$(CDIR)/FP.script 
	ack -c -m$(MACH) $(EMFLAGS) cif8.s

mlf4.$(SUF):	$(CDIR)/mlf4.c
	ack -c.s -m$(MACH) $(EMFLAGS) $(CDIR)/mlf4.c
	ed - mlf4.s <$(CDIR)/FP.script 
	ack -c -m$(MACH) $(EMFLAGS) mlf4.s

mlf8.$(SUF):	$(CDIR)/mlf8.c
	ack -c.s -m$(MACH) $(EMFLAGS) $(CDIR)/mlf8.c
	ed - mlf8.s <$(CDIR)/FP.script 
	ack -c -m$(MACH) $(EMFLAGS) mlf8.s

ngf4.$(SUF):	$(CDIR)/ngf4.c
	ack -c.s -m$(MACH) $(EMFLAGS) $(CDIR)/ngf4.c
	ed - ngf4.s <$(CDIR)/FP.script 
	ack -c -m$(MACH) $(EMFLAGS) ngf4.s

ngf8.$(SUF):	$(CDIR)/ngf8.c
	ack -c.s -m$(MACH) $(EMFLAGS) $(CDIR)/ngf8.c
	ed - ngf8.s <$(CDIR)/FP.script 
	ack -c -m$(MACH) $(EMFLAGS) ngf8.s

sbf4.$(SUF):	$(CDIR)/sbf4.c
	ack -c.s -m$(MACH) $(EMFLAGS) $(CDIR)/sbf4.c
	ed - sbf4.s <$(CDIR)/FP.script 
	ack -c -m$(MACH) $(EMFLAGS) sbf4.s

sbf8.$(SUF):	$(CDIR)/sbf8.c
	ack -c.s -m$(MACH) $(EMFLAGS) $(CDIR)/sbf8.c
	ed - sbf8.s <$(CDIR)/FP.script 
	ack -c -m$(MACH) $(EMFLAGS) sbf8.s

adf4.$(SUF):	$(CDIR)/adf4.c
	ack -c.s -m$(MACH) $(EMFLAGS) $(CDIR)/adf4.c
	ed - adf4.s <$(CDIR)/FP.script 
	ack -c -m$(MACH) $(EMFLAGS) adf4.s

adf8.$(SUF):	$(CDIR)/adf8.c
	ack -c.s -m$(MACH) $(EMFLAGS) $(CDIR)/adf8.c
	ed - adf8.s <$(CDIR)/FP.script 
	ack -c -m$(MACH) $(EMFLAGS) adf8.s

zrf4.$(SUF):	$(CDIR)/zrf4.c
	ack -c.s -m$(MACH) $(EMFLAGS) $(CDIR)/zrf4.c
	ed - zrf4.s <$(CDIR)/FP.script 
	ack -c -m$(MACH) $(EMFLAGS) zrf4.s

zrf8.$(SUF):	$(CDIR)/zrf8.c
	ack -c.s -m$(MACH) $(EMFLAGS) $(CDIR)/zrf8.c
	ed - zrf8.s <$(CDIR)/FP.script 
	ack -c -m$(MACH) $(EMFLAGS) zrf8.s

extend.$(SUF):	$(CDIR)/extend.c
	ack -c.s -m$(MACH) $(EMFLAGS) $(CDIR)/extend.c
	ed - extend.s <$(CDIR)/FP.script 
	ack -c -m$(MACH) $(EMFLAGS) extend.s

compact.$(SUF):	$(CDIR)/compact.c
	ack -c.s -m$(MACH) $(EMFLAGS) $(CDIR)/compact.c
	ed - compact.s <$(CDIR)/FP.script 
	ack -c -m$(MACH) $(EMFLAGS) compact.s

add_ext.$(SUF):	$(CDIR)/add_ext.c
	ack -c.s -m$(MACH) $(EMFLAGS) $(CDIR)/add_ext.c
	ed - add_ext.s <$(CDIR)/FP.script 
	ack -c -m$(MACH) $(EMFLAGS) add_ext.s

div_ext.$(SUF):	$(CDIR)/div_ext.c
	ack -c.s -m$(MACH) $(EMFLAGS) $(CDIR)/div_ext.c
	ed - div_ext.s <$(CDIR)/FP.script 
	ack -c -m$(MACH) $(EMFLAGS) div_ext.s

mul_ext.$(SUF):	$(CDIR)/mul_ext.c
	ack -c.s -m$(MACH) $(EMFLAGS) $(CDIR)/mul_ext.c
	ed - mul_ext.s <$(CDIR)/FP.script 
	ack -c -m$(MACH) $(EMFLAGS) mul_ext.s

nrm_ext.$(SUF):	$(CDIR)/nrm_ext.c
	ack -c.s -m$(MACH) $(EMFLAGS) $(CDIR)/nrm_ext.c
	ed - nrm_ext.s <$(CDIR)/FP.script 
	ack -c -m$(MACH) $(EMFLAGS) nrm_ext.s

sft_ext.$(SUF):	$(CDIR)/sft_ext.c
	ack -c.s -m$(MACH) $(EMFLAGS) $(CDIR)/sft_ext.c
	ed - sft_ext.s <$(CDIR)/FP.script 
	ack -c -m$(MACH) $(EMFLAGS) sft_ext.s

sub_ext.$(SUF):	$(CDIR)/sub_ext.c
	ack -c.s -m$(MACH) $(EMFLAGS) $(CDIR)/sub_ext.c
	ed - sub_ext.s <$(CDIR)/FP.script 
	ack -c -m$(MACH) $(EMFLAGS) sub_ext.s

zrf_ext.$(SUF):	$(CDIR)/zrf_ext.c
	ack -c.s -m$(MACH) $(EMFLAGS) $(CDIR)/zrf_ext.c
	ed - zrf_ext.s <$(CDIR)/FP.script 
	ack -c -m$(MACH) $(EMFLAGS) zrf_ext.s

adder.$(SUF):	$(CDIR)/adder.c
	ack -c.s -m$(MACH) $(EMFLAGS) $(CDIR)/adder.c
	ed - adder.s <$(CDIR)/FP.script 
	ack -c -m$(MACH) $(EMFLAGS) adder.s

shifter.$(SUF):	$(CDIR)/shifter.c
	ack -c.s -m$(MACH) $(EMFLAGS) $(CDIR)/shifter.c
	ed - shifter.s <$(CDIR)/FP.script 
	ack -c -m$(MACH) $(EMFLAGS) shifter.s
