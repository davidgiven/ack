EMHOME = ../..
EMH = $(EMHOME)/h
EMBIN = $(EMHOME)/bin
LIB = $(EMHOME)/modules/lib

LIBS =	$(LIB)/libobject.a $(LIB)/libprint.a \
	$(LIB)/libstring.a $(LIB)/libsystem.a

CFLAGS=-O -I$(EMH) -DDISTRIBUTION
LDFLAGS = 

all:            arch aal

arch:		arch.o
		$(CC) $(LDFLAGS) -o arch arch.o $(LIBS)

aal:		aal.o
		$(CC) $(LDFLAGS) -o aal aal.o $(LIBS)

arch.o:         $(EMH)/arch.h archiver.c
		$(CC) $(CFLAGS) -c archiver.c
		mv archiver.o arch.o

aal.o:		$(EMH)/arch.h archiver.c $(EMH)/ranlib.h $(EMH)/out.h
		$(CC) -DAAL $(CFLAGS) -c archiver.c
		mv archiver.o aal.o

clean:
		rm -f aal arch *.o *.old

install :       all
		rm -f $(EMBIN)/arch $(EMBIN)/aal
		cp aal $(EMBIN)/aal
		cp arch $(EMBIN)/arch
		rm -f $(EMHOME)/man/arch.1 $(EMHOME)/man/aal.1 $(EMHOME)/man/arch.5
		cp aal.1 $(EMHOME)/man/aal.1
		cp arch.1 $(EMHOME)/man/arch.1
		cp arch.5 $(EMHOME)/man/arch.5

cmp :           all
		-cmp aal $(EMBIN)/aal
		-cmp arch $(EMBIN)/arch
		-cmp aal.1 $(EMHOME)/man/aal.1
		-cmp arch.1 $(EMHOME)/man/arch.1
		-cmp arch.5 $(EMHOME)/man/arch.5

opr:
		make pr ^ opr
pr:
		@pr Makefile archiver.c
