EMHOME=../../..
OBJLIB=$(EMHOME)/modules/lib/libobject.a

head:	cv

cv:	cv.c
	$(CC) $(CFLAGS) $(LDFLAGS) -I$(EMHOME)/h -o cv cv.c $(OBJLIB)

install:	head
	../../install cv

cmp:	head
	../../compare cv

clean:
	rm -f *.o

pr:
	@pr `pwd`/Makefile `pwd`/cv.c

opr:
	make pr | opr
