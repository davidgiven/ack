# $Header$

#PARAMS		do not remove this line!

SRC_DIR = $(SRC_HOME)/modules/src/string
MOD_DIR = $(TARGET_HOME)/modules

SRC =	$(SRC_DIR)/bts2str.c $(SRC_DIR)/btscat.c $(SRC_DIR)/btscmp.c \
	$(SRC_DIR)/btscpy.c $(SRC_DIR)/btszero.c $(SRC_DIR)/long2str.c \
	$(SRC_DIR)/str2bts.c $(SRC_DIR)/str2long.c $(SRC_DIR)/strcat.c \
	$(SRC_DIR)/strcmp.c $(SRC_DIR)/strcpy.c $(SRC_DIR)/strindex.c \
	$(SRC_DIR)/strlen.c $(SRC_DIR)/strncat.c $(SRC_DIR)/strncmp.c \
	$(SRC_DIR)/strncpy.c $(SRC_DIR)/strrindex.c $(SRC_DIR)/strzero.c

OBJ =	bts2str.$(SUF) btscat.$(SUF) btscmp.$(SUF) btscpy.$(SUF) \
	btszero.$(SUF) long2str.$(SUF) str2bts.$(SUF) str2long.$(SUF) \
	strcat.$(SUF) strcmp.$(SUF) strcpy.$(SUF) strindex.$(SUF) \
	strlen.$(SUF) strncat.$(SUF) strncmp.$(SUF) strncpy.$(SUF) \
	strrindex.$(SUF) strzero.$(SUF)

INCLUDES = -I$(SRC_DIR)
CFLAGS = $(COPTIONS) $(INCLUDES)

LIBSTRING = libstring.$(LIBSUF)

all:		$(LIBSTRING)

$(LIBSTRING):	$(OBJ)
		$(AR) r $(LIBSTRING) $(OBJ)
		$(RANLIB) $(LIBSTRING)

install:	all
		cp $(LIBSTRING) $(MOD_DIR)/lib/$(LIBSTRING)
		$(RANLIB) $(MOD_DIR)/lib/$(LIBSTRING)
		cp $(SRC_DIR)/string.3 $(MOD_DIR)/man/string.3
		if [ $(DO_MACHINE_INDEP) = y ] ; \
		then	mk_manpage $(SRC_DIR)/string.3 $(TARGET_HOME) ; \
		fi

cmp:		all
		-cmp $(LIBSTRING) $(MOD_DIR)/lib/$(LIBSTRING)
		-cmp $(SRC_DIR)/string.3 $(MOD_DIR)/man/string.3

pr:
		@pr $(SRC_DIR)/proto.make $(SRC)

opr:
		make pr | opr

clean:
		rm -f *.$(SUF) *.$(LIBSUF)

lintlib:
		$(MK_LINT_LIB) string $(MOD_DIR)/lib $(INCLUDES) $(SRC)

bts2str.$(SUF):	$(SRC_DIR)/bts2str.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/bts2str.c

btscat.$(SUF):	$(SRC_DIR)/btscat.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/btscat.c

btscmp.$(SUF):	$(SRC_DIR)/btscmp.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/btscmp.c

btscpy.$(SUF):	$(SRC_DIR)/btscpy.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/btscpy.c

btszero.$(SUF):	$(SRC_DIR)/btszero.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/btszero.c

long2str.$(SUF):	$(SRC_DIR)/long2str.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/long2str.c

str2bts.$(SUF):	$(SRC_DIR)/str2bts.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/str2bts.c

str2long.$(SUF):	$(SRC_DIR)/str2long.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/str2long.c

strcat.$(SUF):	$(SRC_DIR)/strcat.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/strcat.c

strcmp.$(SUF):	$(SRC_DIR)/strcmp.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/strcmp.c

strcpy.$(SUF):	$(SRC_DIR)/strcpy.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/strcpy.c

strindex.$(SUF):	$(SRC_DIR)/strindex.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/strindex.c

strlen.$(SUF):	$(SRC_DIR)/strlen.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/strlen.c

strncat.$(SUF):	$(SRC_DIR)/strncat.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/strncat.c

strncmp.$(SUF):	$(SRC_DIR)/strncmp.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/strncmp.c

strncpy.$(SUF):	$(SRC_DIR)/strncpy.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/strncpy.c

strrindex.$(SUF):	$(SRC_DIR)/strrindex.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/strrindex.c

strzero.$(SUF):	$(SRC_DIR)/strzero.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/strzero.c
