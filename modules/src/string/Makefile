EMHOME = ../../..
MODULES = $(EMHOME)/modules
INSTALL = $(MODULES)/install
COMPARE = $(MODULES)/compare
AR	= ar
SUF	= o
LIBSUF	= a

CFLAGS = -O $(COPT)

SRC =	bts2str.c btscat.c btscmp.c btscpy.c btszero.c long2str.c \
	str2bts.c str2long.c strcat.c strcmp.c strcpy.c strindex.c \
	strlen.c strncat.c strncmp.c strncpy.c strrindex.c strzero.c
OBJ =	bts2str.$(SUF) btscat.$(SUF) btscmp.$(SUF) btscpy.$(SUF) \
	btszero.$(SUF) long2str.$(SUF) str2bts.$(SUF) str2long.$(SUF) \
	strcat.$(SUF) strcmp.$(SUF) strcpy.$(SUF) strindex.$(SUF) \
	strlen.$(SUF) strncat.$(SUF) strncmp.$(SUF) strncpy.$(SUF) \
	strrindex.$(SUF) strzero.$(SUF)

.SUFFIXES: .$(SUF)
.c.$(SUF):
	$(CC) -c $(CFLAGS) $*.c

all:		libstring.$(LIBSUF)

libstring.$(LIBSUF):	$(OBJ) Makefile
		$(AR) r libstring.$(LIBSUF) $(OBJ)
		-sh -c 'ranlib libstring.$(LIBSUF)'

install:	all
		$(INSTALL) lib/libstring.$(LIBSUF)
		$(INSTALL) man/string.3

cmp:		all
		-$(COMPARE) lib/libstring.$(LIBSUF)
		-$(COMPARE) man/string.3

pr:
		@pr Makefile $(SRC)

opr:
		make pr | opr

clean:
		rm -f *.$(SUF) *.$(LIBSUF)

lintlib:
		lint -Cstring $(SRC)
		mv llib-lstring.ln $(MODULES)/lib
