# $Header$

#PARAMS		do not remove this line!

SRC_DIR = $(SRC_HOME)/mach/sun3/libce

EMOBJ =	aar.o cii.o cmi.o cms.o cmu.o csa.o csb.o cuu.o dia.o exg.o fat.o \
	inn.o lar.o los.o mon.o nop.o sar.o set.o shp.o sts.o trp.o trpstr.o

FOBJ =	adf4.o adf8.o cff.o cfi.o cfu.o cif.o cmf4.o cmf8.o cuf.o dvf4.o \
	dvf8.o fef4.o fef8.o fif4.o fif8.o mlf4.o mlf8.o sbf4.o sbf8.o vars.o \
	sys.o

OBJ =	$(EMOBJ) $(FOBJ)

all:	libext.a head_em.sun.o

libext.a:	$(OBJ)
	for i in $(OBJ) ; do $(TARGET_HOME)/lib.bin/sun3/cv -u $$i $$i.X ; mv $$i.X $$i ; done
	ar rv libext.a $(OBJ)
	ranlib libext.a

head_em.sun.o:	head_em.o
	$(TARGET_HOME)/lib.bin/sun3/cv -u head_em.o head_em.sun.o

install:	all
	cp libext.a $(TARGET_HOME)/lib/sun3/tail_ext
	ranlib $(TARGET_HOME)/lib/sun3/tail_ext
	cp head_em.sun.o $(TARGET_HOME)/lib/sun3/head_ext

cmp:	all
	-cmp libext.a $(TARGET_HOME)/lib/sun3/tail_ext
	-cmp head_em.sun.o $(TARGET_HOME)/lib/sun3/head_ext

clean:
	rm -f *.[oa]

pr:

opr:

$(EMOBJ):	$(TARGET_HOME)/lib/m68020/tail_em
	aal x $(TARGET_HOME)/lib/m68020/tail_em $(EMOBJ)

$(FOBJ) head_em.o:
	for i in $(SRC_DIR)/*.s ; do sun3 -c $$i ; done
