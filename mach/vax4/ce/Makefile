EMHOME=../../..
BACK=$(EMHOME)/lib/ceg/ce_back

install : back.a
	install_ceg

clean:
	rm -rf back.a back

back.a :
	-mkdir back
	cp $(BACK)/obj_back/*h back
	cp Make.back back/Makefile
	( cd back; make)
	ar r back.a back/*o;
	-ranlib back.a
