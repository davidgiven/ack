EMHOME = ../../..
MODDIR=$(EMHOME)/modules
INSTALL = $(MODDIR)/install
COMPARE = $(MODDIR)/compare
INCLUDES = -I$(MODDIR)/h
CFLAGS = -O $(INCLUDES) $(COPT)
AR = ar
SUF = o
LIBSUF = a

LIBPRINT = libprint.$(LIBSUF)

SRC = doprnt.c fprint.c print.c sprint.c format.c
OBJ = doprnt.$(SUF) fprint.$(SUF) print.$(SUF) sprint.$(SUF) format.$(SUF)

.SUFFIXES: .$(SUF)
.c.$(SUF):
	$(CC) -c $(CFLAGS) $*.c

all:		$(LIBPRINT)

$(LIBPRINT):	$(OBJ)
		rm -f $(LIBPRINT)
		$(AR) r $(LIBPRINT) $(OBJ)
		-sh -c 'ranlib $(LIBPRINT)'

install:	all
		$(INSTALL) lib/$(LIBPRINT)
		$(INSTALL) man/print.3

cmp:		all
		-$(COMPARE) lib/$(LIBPRINT)
		-$(COMPARE) man/print.3

pr:
		@pr Makefile $(SRC)

opr:
		make pr | opr

clean:
		rm -f *.$(SUF) $(LIBPRINT)

lintlib:
		lint $(INCLUDES) -Cprint $(SRC)
		mv llib-lprint.ln $(MODDIR)/lib

doprnt.$(SUF): param.h
fprint.$(SUF): param.h
print.$(SUF): param.h
sprint.$(SUF): param.h
