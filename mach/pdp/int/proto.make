# $Header$

#PARAMS		do not remove this line!

INTS=em_t--- em_---- em_tf-- em_t-c- em_t--p

SRC_DIR = $(SRC_HOME)/mach/pdp/int
CFLAGS = $(COPTIONS) -I$(TARGET_HOME)/h -I$(TARGET_HOME)/config
LDFLAGS = $(LDOPTIONS)

b=$(TARGET_HOME)/lib.bin/em22

all:	$(INTS) eminform em

em:	em.o
	$(CC) $(LDFLAGS) -o em em.c

em.o:	$(SRC_DIR)/em.c
	$(CC) $(CFLAGS) -c $(SRC_DIR)/em.c

eminform:	$(SRC_DIR)/eminform.s
	as $(SRC_DIR)/eminform.s;ld -i -o eminform a.out -lc

em_t---:	$(SRC_DIR)/t+ $(SRC_DIR)/f- $(SRC_DIR)/c- $(SRC_DIR)/p- $(SRC_DIR)/em_int.s
	as $(SRC_DIR)/t+ $(SRC_DIR)/f- $(SRC_DIR)/c- $(SRC_DIR)/p- $(SRC_DIR)/em_int.s;ld -i -o em_t--- a.out -lc

em_----:	$(SRC_DIR)/t- $(SRC_DIR)/f- $(SRC_DIR)/c- $(SRC_DIR)/p- $(SRC_DIR)/em_int.s
	as $(SRC_DIR)/t- $(SRC_DIR)/f- $(SRC_DIR)/c- $(SRC_DIR)/p- $(SRC_DIR)/em_int.s;ld -i -o em_---- a.out -lc

em_tf--:	$(SRC_DIR)/t+ $(SRC_DIR)/f+ $(SRC_DIR)/c- $(SRC_DIR)/p- $(SRC_DIR)/em_int.s
	as $(SRC_DIR)/t+ $(SRC_DIR)/f+ $(SRC_DIR)/c- $(SRC_DIR)/p- $(SRC_DIR)/em_int.s;ld -i -o em_tf-- a.out -lc

em_t-c-:	$(SRC_DIR)/t+ $(SRC_DIR)/f- $(SRC_DIR)/c+ $(SRC_DIR)/p- $(SRC_DIR)/em_int.s
	as $(SRC_DIR)/t+ $(SRC_DIR)/f- $(SRC_DIR)/c+ $(SRC_DIR)/p- $(SRC_DIR)/em_int.s;ld -i -o em_t-c- a.out -lc

em_t--p:	$(SRC_DIR)/t+ $(SRC_DIR)/f- $(SRC_DIR)/c- $(SRC_DIR)/p+ $(SRC_DIR)/em_int.s
	as $(SRC_DIR)/t+ $(SRC_DIR)/f- $(SRC_DIR)/c- $(SRC_DIR)/p+ $(SRC_DIR)/em_int.s;ld -i -o em_t--p a.out -lc

install:	all
		-mkdir $b
		cp em_???? $b
		cp em eminform $(TARGET_HOME)/bin
		if [ $(DO_MACHINE_INDEP) = y ] ; \
		then	mk_manpage $(SRC_DIR)/em.1 $(TARGET_HOME) ; \
			mk_manpage $(SRC_DIR)/eminform.1 $(TARGET_HOME) ; \
		fi

cmp: 		all
		-cmp em_t--- $b/em_t---
		-cmp em_---- $b/em_----
		-cmp em_tf-- $b/em_tf--
		-cmp em_t-c- $b/em_t-c-
		-cmp em_t--p $b/em_t--p
		-cmp em $(TARGET_HOME)/bin/em
		-cmp eminform $(TARGET_HOME)/bin/eminform

clean:
		-rm -f *.o *.old a.out em eminform $(INTS)

opr:
		make pr | opr

pr:
		@pr $(SRC_DIR)/em.c $(SRC_DIR)/em_int.s $(SRC_DIR)/eminform.s
