# $Header$

#PARAMS		do not remove this line!

SRC_DIR = $(SRC_HOME)/modules/src/input
MOD_DIR = $(TARGET_HOME)/modules

OBJECTS =	AtEoIF.$(SUF)\
		AtEoIT.$(SUF)

CFLAGS = $(COPTIONS)

all:		libinput.$(LIBSUF)

libinput.$(LIBSUF):	$(OBJECTS)
		rm -f libinput.$(LIBSUF)
		$(AR) r libinput.$(LIBSUF) $(OBJECTS)
		$(RANLIB) libinput.$(LIBSUF)

install:	all
		-mkdir $(MOD_DIR)/lib
		-mkdir $(MOD_DIR)/pkg
		cp libinput.$(LIBSUF) $(MOD_DIR)/lib/libinput.$(LIBSUF)
		$(RANLIB) $(MOD_DIR)/lib/libinput.$(LIBSUF)
		cp $(SRC_DIR)/inp_pkg.body $(MOD_DIR)/pkg/inp_pkg.body
		cp $(SRC_DIR)/inp_pkg.spec $(MOD_DIR)/pkg/inp_pkg.spec
		if [ $(DO_MACHINE_INDEP) = y ] ; \
		then	mk_manpage $(SRC_DIR)/input.3 $(TARGET_HOME) ; \
		fi

cmp:		all
		-cmp libinput.$(LIBSUF) $(MOD_DIR)/lib/libinput.$(LIBSUF)
		-cmp $(SRC_DIR)/inp_pkg.body $(MOD_DIR)/pkg/inp_pkg.body
		-cmp $(SRC_DIR)/inp_pkg.spec $(MOD_DIR)/pkg/inp_pkg.spec

pr:
		@pr $(SRC_DIR)/proto.make $(SRC_DIR)/inp_pkg.spec $(SRC_DIR)/inp_pkg.body $(SRC_DIR)/AtEoIF.c $(SRC_DIR)/AtEoIT.c

opr:
		make pr | opr

clean:
		rm -f *.$(SUF) *.$(LIBSUF)

lintlib:
		$(MK_LINT_LIB) input $(MOD_DIR)/lib $(SRC_DIR)/AtEoIF.c $(SRC_DIR)/AtEoIT.c

AtEoIT.$(SUF):	$(SRC_DIR)/AtEoIT.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/AtEoIT.c

AtEoIF.$(SUF):	$(SRC_DIR)/AtEoIF.c
		$(CC) -c $(CFLAGS) $(SRC_DIR)/AtEoIF.c
