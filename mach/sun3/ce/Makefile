EMHOME=../../..
BACK=$(EMHOME)/lib/ceg/ce_back

all : back.a
	make -f ce_makefile

install:	all
	make -f ce_makefile install

clean:
	rm -rf back.a back

back.a :
	-mkdir back
	cp $(BACK)/obj_back/*h back
	cp Make.back back/Makefile
	( cd back; make)
	ar r back.a back/*o;
	-ranlib back.a
