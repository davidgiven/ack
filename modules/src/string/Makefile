EMHOME = ../../..
MODULES = $(EMHOME)/modules
INSTALL = $(MODULES)/install
COMPARE = $(MODULES)/compare
AR	= ar

CFLAGS = -O

SRC =	bts2str.c btscat.c btscmp.c btscpy.c btszero.c long2str.c \
	str2bts.c str2long.c strcat.c strcmp.c strcpy.c strindex.c \
	strlen.c strncat.c strncmp.c strncpy.c strrindex.c strzero.c
OBJ =	bts2str.o btscat.o btscmp.o btscpy.o btszero.o long2str.o \
	str2bts.o str2long.o strcat.o strcmp.o strcpy.o strindex.o \
	strlen.o strncat.o strncmp.o strncpy.o strrindex.o strzero.o

all:		libstring.a

libstring.a:	$(OBJ) Makefile
		$(AR) r libstring.a $(OBJ)
		-sh -c 'ranlib libstring.a'

install:	all
		$(INSTALL) lib/libstring.a
		$(INSTALL) man/string.3

cmp:		all
		$(COMPARE) lib/libstring.a
		$(COMPARE) man/string.3

pr:
		@pr Makefile $(SRC)

opr:
		make pr | opr

clean:
		rm -f *.[oa]

lintlib:
		lint -Cstring $(SRC)
		mv llib-lstring.ln $(MODULES)/lib
