# $Header$

#PARAMS		do not remove this line!

MACH = vax4
OBJ = obj
SRC_DIR = $(SRC_HOME)/mach/$(MACH)/ce
BACK=$(TARGET_HOME)/lib.bin/ceg/ce_back

CEG = $(TARGET_HOME)/lib.bin/ceg/util

all:	back_vax.$(LIBSUF)
	make -f $(CEG)/make_asobj "OBJ="$(OBJ) "MACH="$(MACH)

install:	install_vax
	make -f $(CEG)/make_asobj "OBJ="$(OBJ) "MACH="$(MACH) install

cmp:	back_vax.$(LIBSUF)
	-cmp back_vax.$(LIBSUF) $(TARGET_HOME)/lib.bin/vax4/back_vax.$(LIBSUF)
	-make -f $(CEG)/make_asobj "OBJ="$(OBJ) "MACH="$(MACH) cmp

install_vax:	back_vax.$(LIBSUF)
	cp back_vax.$(LIBSUF) $(TARGET_HOME)/lib.bin/vax4/back_vax.$(LIBSUF)
	$(RANLIB) $(TARGET_HOME)/lib.bin/vax4/back_vax.$(LIBSUF)

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
	make -f $(CEG)/make_asobj "OBJ="$(OBJ) clean
	rm -rf back_vax back_vax.$(LIBSUF)

# only remove ce, ceg, and back directories
dclean:
	make -f $(CEG)/make_asobj "OBJ="$(OBJ) dclean
	rm -rf back_vax

back_vax.$(LIBSUF):	$(SRC_DIR)/do_close.c $(SRC_DIR)/do_open.c \
			$(SRC_DIR)/end_back.c $(SRC_DIR)/output.c \
			$(SRC_DIR)/relocation.c
	-mkdir back_vax
	cp $(BACK)/obj_back/*h back_vax
	cp $(SRC_DIR)/Make.back back_vax/Makefile
	cd back_vax; make SRC_DIR=$(SRC_DIR) CC="$(CC)" CFLAGS="$(COPTIONS)" TARGET_HOME=$(TARGET_HOME) ; cd ..
	$(AR) r back_vax.$(LIBSUF) back_vax/*$(SUF)
	$(RANLIB) back_vax.$(LIBSUF)
