EMHOME = ../../..
LIBPRINT = libprint.a
MODDIR=$(EMHOME)/modules
INSTALL = $(MODDIR)/install
COMPARE = $(MODDIR)/compare
INCLUDES = -I$(MODDIR)/h
CFLAGS = -O $(INCLUDES)
AR = ar
SRC = doprnt.c fprint.c print.c sprint.c format.c
OBJ = doprnt.o fprint.o print.o sprint.o format.o

all:		$(LIBPRINT)

$(LIBPRINT):	$(OBJ)
		rm -f $(LIBPRINT)
		$(AR) r $(LIBPRINT) $(OBJ)
		-sh -c 'ranlib $(LIBPRINT)'

install:	all
		$(INSTALL) lib/$(LIBPRINT)
		$(INSTALL) man/print.3

cmp:		all
		$(COMPARE) lib/$(LIBPRINT)
		$(COMPARE) man/print.3

pr:
		@pr Makefile $(SRC)

opr:
		make pr | opr

clean:
		rm -f *.o $(LIBPRINT)

lintlib:
		lint $(INCLUDES) -Cprint $(SRC)
		mv llib-lprint.ln $(MODDIR)/lib

doprnt.o: param.h
fprint.o: param.h
print.o: param.h
sprint.o: param.h
