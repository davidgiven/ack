EMHOME = ../../..
MODULES = $(EMHOME)/modules
INSTALL = $(MODULES)/install
COMPARE = $(MODULES)/compare
INCLUDES = -I.
CFLAGS	= $(INCLUDES) -O $(COPT)
AR = ar
SUF = o
LIBSUF = a

LIBSYS	= libsystem.$(LIBSUF)
OBJ	= access.$(SUF) break.$(SUF) chmode.$(SUF) close.$(SUF) create.$(SUF) \
	  filesize.$(SUF) modtime.$(SUF) lock.$(SUF) open.$(SUF) read.$(SUF) \
	  remove.$(SUF) stop.$(SUF) system.$(SUF) time.$(SUF) unlock.$(SUF) \
	  write.$(SUF) seek.$(SUF) rename.$(SUF)
CSRC	= access.c break.c chmode.c close.c create.c filesize.c \
	  modtime.c lock.c open.c read.c remove.c stop.c \
	  system.c time.c unlock.c write.c seek.c rename.c
SRC	= Makefile system.h $(CSRC)

.SUFFIXES: .$(SUF)
.c.$(SUF):
	$(CC) -c $(CFLAGS) $*.c

all:		$(LIBSYS)

$(LIBSYS):	$(OBJ)
		rm -f $(LIBSYS)
		$(AR) r $(LIBSYS) $(OBJ)
		-sh -c 'ranlib $(LIBSYS)'

install:	all
		$(INSTALL) lib/$(LIBSYS)
		$(INSTALL) man/system.3
		$(INSTALL) h/system.h

cmp:		all
		-$(COMPARE) lib/$(LIBSYS)
		-$(COMPARE) man/system.3
		-$(COMPARE) h/system.h

clean:
		rm -f *.$(SUF) *.$(LIBSUF)

pr:
		@pr $(SRC)

opr:
		make pr | opr

lintlib:
		lint $(INCLUDES) -Csystem $(CSRC)
		mv llib-lsystem.ln $(MODULES)/lib

access.$(SUF): system.h
break.$(SUF): system.h
close.$(SUF): system.h
create.$(SUF): system.h
open.$(SUF): system.h
read.$(SUF): system.h
stop.$(SUF): system.h
system.$(SUF): system.h
write.$(SUF): system.h
