EMHOME = ../../..
MODULES = $(EMHOME)/modules
INSTALL = $(MODULES)/install
COMPARE = $(MODULES)/compare
LIBSYS	= libsystem.a
CFLAGS	= -I. -O
OBJ	= access.o break.o chmode.o close.o create.o exit.o filesize.o \
	modtime.o lock.o open.o read.o remove.o stop.o \
	system.o time.o unlock.o write.o
CSRC	= access.c break.c chmode.c close.c create.c exit.c filesize.c \
	modtime.c lock.c open.c read.c remove.c stop.c \
	system.c time.c unlock.c write.c
SRC	= Makefile system.h $(CSRC)

all:		$(LIBSYS)

$(LIBSYS):	$(OBJ)
		rm -f $(LIBSYS)
		ar r $(LIBSYS) $(OBJ)
		-sh -c 'ranlib $(LIBSYS)'

install:	all
		$(INSTALL) lib/$(LIBSYS)
		$(INSTALL) man/system.3
		$(INSTALL) h/system.h

cmp:		all
		$(COMPARE) lib/$(LIBSYS)
		$(COMPARE) man/system.3
		$(COMPARE) h/system.h

clean:
		rm -f *.[oa]

pr:
		@pr $(SRC)

opr:
		make pr | opr

access.o: system.h
break.o: system.h
close.o: system.h
create.o: system.h
open.o: system.h
read.o: system.h
stop.o: system.h
system.o: system.h
write.o: system.h
