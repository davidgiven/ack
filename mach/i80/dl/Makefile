EMHOME=../../..
OBJLIB=$(EMHOME)/modules/lib/libobject.a

head:	mccpm nascom

mccpm:	mccpm.c
	$(CC) -I$(EMHOME)/h -o mccpm mccpm.c $(OBJLIB)

nascom:	nascom.c
	$(CC) -I$(EMHOME)/h -o nascom nascom.c $(OBJLIB)

install:	head
	@echo Nothing is installed

cmp:	head
	@echo Nothing is compared

clean:
	rm -f *.o

pr:
	@pr `pwd`/Makefile `pwd`/mccpm.c `pwd`/nascom.c

opr:
	make pr | opr
