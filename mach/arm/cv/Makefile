EMHOME=../../..
OBJLIB=$(EMHOME)/modules/lib/libobject.a

head:	cv

cv:	conv.c
	$(CC) -I$(EMHOME)/h -o cv cv.c $(OBJLIB)

install:	head
	@echo Nothing is installed

cmp:	head
	@echo Nothing is compared

clean:
	rm -f *.o

pr:
	@pr `pwd`/Makefile `pwd`/cv.c

opr:
	make pr | opr
