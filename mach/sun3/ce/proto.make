# $Id$

#PARAMS		do not remove this line!

MACH = sun3
OBJ = obj
SRC_DIR = $(SRC_HOME)/mach/$(MACH)/ce
BACK=$(TARGET_HOME)/lib.bin/ceg/ce_back

CEG = $(TARGET_HOME)/lib.bin/ceg/util

all:	back.$(LIBSUF)
	make -f $(CEG)/make_own "MACH="$(MACH)

install:	back.$(LIBSUF)
	make -f $(CEG)/make_own "MACH="$(MACH) install

cmp:	back.$(LIBSUF)
	-make -f $(CEG)/make_own "OBJ="$(OBJ) "MACH="$(MACH) cmp

pr:
	@pr $(SRC_DIR)/proto.make $(SRC_DIR)/EM_table $(SRC_DIR)/mach.h \
	    $(SRC_DIR)/mach.c $(SRC_DIR)/as_table $(SRC_DIR)/as.h \
	    $(SRC_DIR)/as.c $(SRC_DIR)/Make.back $(SRC_DIR)/do_close.c \
	    $(SRC_DIR)/do_open.c $(SRC_DIR)/end_back.c \
	    $(SRC_DIR)/output.c $(SRC_DIR)/relocation.c

opr:
	make pr | opr

# total cleanup
clean:
	make -f $(CEG)/make_own "OBJ="$(OBJ) clean
	rm -rf back back.$(LIBSUF)

# only remove ce, ceg, and back directories
dclean:
	make -f $(CEG)/make_own "OBJ="$(OBJ) dclean
	rm -rf back

back.$(LIBSUF):	$(SRC_DIR)/do_close.c $(SRC_DIR)/do_open.c \
			$(SRC_DIR)/end_back.c $(SRC_DIR)/output.c \
			$(SRC_DIR)/relocation.c
	-mkdir back
	cp $(BACK)/obj_back/*h back
	cp $(SRC_DIR)/Make.back back/Makefile
	cd back; make SRC_DIR=$(SRC_DIR) CC="$(CC)" CFLAGS="$(COPTIONS)" TARGET_HOME=$(TARGET_HOME) ; cd ..
	$(AR) r back.$(LIBSUF) back/*$(SUF)
	$(RANLIB) back.$(LIBSUF)
