# $Id$

#PARAMS         do not remove this line!

SRC_DIR = $(SRC_HOME)/doc/em/int

install \
all:            em emdmp tables

tables:         mktables $(SRC_HOME)/etc/ip_spec.t
		mktables $(SRC_HOME)/etc/ip_spec.t tables

mktables:       $(SRC_DIR)/mktables.c $(TARGET_HOME)/h/em_spec.h \
		$(TARGET_HOME)/h/em_flag.h \
		$(TARGET_HOME)/lib.bin/em_data.$(LIBSUF) $(TARGET_HOME)/h/ip_spec.h
		$(CC) -I$(TARGET_HOME)/h -O -o mktables $(SRC_DIR)/mktables.c $(TARGET_HOME)/lib.bin/em_data.$(LIBSUF)

em:             $(SRC_DIR)/em.p
		apc -O $(SRC_DIR)/em.p >emerrs ; mv a.out em

nem.p:          $(SRC_DIR)/em.p
		sed -e '/maxadr  = t16/s//maxadr  =t15/' -e '/maxdata = 8191; /s//maxdata = 14335;/' -e '/ adr=.*long/s// adr=    0..maxadr/' <$(SRC_DIR)/em.p >nem.p

nem:            nem.p
		apc -O nem.p >emerrs ; mv a.out nem

emdmp:          $(SRC_DIR)/emdmp.c
		$(CC) -I$(TARGET_HOME)/h -I$(TARGET_HOME)/config -o emdmp -O $(SRC_DIR)/emdmp.c

cmp:

pr:		
		@pr $(SRC_DIR)/em.p $(SRC_DIR)/mktables.c $(SRC_DIR)/emdmp.c
