# $Header$

#PARAMS		do not remove this line!

SRC_DIR = $(SRC_HOME)/mach/vax4/libce

FOBJ = sys.o

all:	libext.a

libext.a:	$(TARGET_HOME)/lib/vax4/tail_em $(FOBJ)
	aal x $(TARGET_HOME)/lib/vax4/tail_em
	for i in *.o ; do $(TARGET_HOME)/lib.bin/vax4/cv -u $$i $$i.X ; mv $$i.X $$i ; done
	ar rv libext.a *.o
	ranlib libext.a

$(FOBJ):	
	for i in $(SRC_DIR)/*.s ; do vax4 -c $$i ; done

install:	all
	cp libext.a $(TARGET_HOME)/lib/vax4/tail_ext
	ranlib $(TARGET_HOME)/lib/vax4/tail_ext

cmp:	all
	-cmp libext.a $(TARGET_HOME)/lib/vax4/tail_ext

clean:
	rm -f *.[oa]

pr:

opr:
