EMHOME=../../..
OBJLIB=$(EMHOME)/modules/lib/libobject.a

head:	dl

dl:	dl.c
	$(CC) -I$(EMHOME)/h -o dl dl.c $(OBJLIB)

install:	head
	@echo Nothing is installed

cmp:	head
	@echo Nothing is compared

clean:
	rm -f *.o

pr:
	@pr `pwd`/Makefile `pwd`/dl.c

opr:
	make pr | opr
