# $Header$

#PARAMS		do not remove this line!

SRC_DIR = $(SRC_HOME)/modules/src/assert
MOD_DIR = $(TARGET_HOME)/modules
INCLUDES = -I$(SRC_DIR) -I$(MOD_DIR)/h
CFLAGS = $(COPTIONS) $(INCLUDES)

all:		libassert.$(LIBSUF)

libassert.$(LIBSUF):	BadAssert.$(SUF)
		$(AR) r libassert.$(LIBSUF) BadAssert.$(SUF)
		$(RANLIB) libassert.$(LIBSUF)

install:	all
		cp libassert.$(LIBSUF) $(MOD_DIR)/lib/libassert.$(LIBSUF)
		$(RANLIB) $(MOD_DIR)/lib/libassert.$(LIBSUF)
		cp $(SRC_DIR)/assert.3 $(MOD_DIR)/man/assert.3
		cp $(SRC_DIR)/assert.h $(MOD_DIR)/h/assert.h
		if [ $(DO_MACHINE_INDEP) = y ] ; \
		then	mk_manpage $(SRC_DIR)/assert.3 $(TARGET_HOME) ; \
		fi

cmp:		all
		-cmp libassert.$(LIBSUF) $(MOD_DIR)/lib/libassert.$(LIBSUF)
		-cmp $(SRC_DIR)/assert.h $(MOD_DIR)/h/assert.h
		-cmp $(SRC_DIR)/assert.3 $(MOD_DIR)/man/assert.3

pr:
		@pr $(SRC_DIR)/proto.make $(SRC_DIR)/assert.h $(SRC_DIR)/BadAssert.c

opr:
		make pr | opr

clean:
		rm -f *.$(LIBSUF) *.$(SUF)

lintlib:
		$(MK_LINT_LIB) assert $(MOD_DIR)/lib $(INCLUDES) $(SRC_DIR)/BadAssert.c

BadAssert.$(SUF):	$(SRC_DIR)/BadAssert.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/BadAssert.c
