# $Header$

#PARAMS		do not remove this line!

SRC_DIR = $(SRC_HOME)/modules/src/print
MOD_DIR = $(TARGET_HOME)/modules
LIBPRINT = libprint.$(LIBSUF)
INCLUDES = -I$(SRC_DIR) -I$(MOD_DIR)/h

CFLAGS = $(COPTIONS) $(INCLUDES)

SRC =	$(SRC_DIR)/doprnt.c $(SRC_DIR)/fprint.c $(SRC_DIR)/print.c \
	$(SRC_DIR)/sprint.c $(SRC_DIR)/format.c
OBJ =	doprnt.$(SUF) fprint.$(SUF) print.$(SUF) sprint.$(SUF) format.$(SUF)

all:		$(LIBPRINT)

$(LIBPRINT):	$(OBJ)
		rm -f $(LIBPRINT)
		$(AR) r $(LIBPRINT) $(OBJ)
		$(RANLIB) $(LIBPRINT)

install:	all lintlib
		cp $(LIBPRINT) $(MOD_DIR)/lib/$(LIBPRINT)
		$(RANLIB) $(MOD_DIR)/lib/$(LIBPRINT)
		cp $(SRC_DIR)/print.3 $(MOD_DIR)/man/print.3

cmp:		all
		-cmp $(LIBPRINT) $(MOD_DIR)/lib/$(LIBPRINT)
		-cmp $(SRC_DIR)/print.3 $(MOD_DIR)/man/print.3

pr:
		@pr $(SRC_DIR)/proto.make $(SRC)

opr:
		make pr | opr

clean:
		rm -f *.$(SUF) $(LIBPRINT)

lintlib:
		$(MK_LINT_LIB) print $(MOD_DIR)/lib $(INCLUDES) $(SRC)

doprnt.$(SUF):	$(SRC_DIR)/param.h $(SRC_DIR)/doprnt.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/doprnt.c

fprint.$(SUF):	$(SRC_DIR)/param.h $(SRC_DIR)/fprint.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/fprint.c

print.$(SUF):	$(SRC_DIR)/param.h $(SRC_DIR)/print.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/print.c

sprint.$(SUF):	$(SRC_DIR)/param.h $(SRC_DIR)/sprint.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/sprint.c

format.$(SUF):	$(SRC_DIR)/format.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/format.c
