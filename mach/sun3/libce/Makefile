# $Header$

EMHOME=../../..

EMOBJ =	aar.o cii.o cmi.o cms.o cmu.o csa.o csb.o cuu.o dia.o exg.o fat.o \
	inn.o lar.o los.o mon.o nop.o sar.o set.o shp.o sts.o trp.o trpstr.o

FOBJ =	adf4.o adf8.o cff.o cfi.o cfu.o cif.o cmf4.o cmf8.o cuf.o dvf4.o \
	dvf8.o fef4.o fef8.o fif4.o fif8.o mlf4.o mlf8.o sbf4.o sbf8.o vars.o

OBJ =	$(EMOBJ) $(FOBJ)
all:	libext.a

.s.o:
	sun3 -c $*.s

libext.a:	$(OBJ)
	for i in $(OBJ) ; do ../../../lib/sun3/cv -u $$i $$i.X ; mv $$i.X $$i ; done
	ar rv libext.a $(OBJ)
	ranlib libext.a

install:	all
	../../install libext.a tail_ext

cmp:	all
	-../../compare libext.a tail_ext

clean:
	rm -f *.[oa]

pr:

opr:

$(EMOBJ):	$(EMHOME)/lib/m68020/tail_em
	aal x $(EMHOME)/lib/m68020/tail_em $(EMOBJ)
