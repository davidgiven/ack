CFLAGS=-O
HOME=../../..

install \
all:            em emdmp tables

tables:         mktables $(HOME)/etc/ip_spec.t
		mktables $(HOME)/etc/ip_spec.t tables

mktables:       mktables.c $(HOME)/h/em_spec.h $(HOME)/h/em_flag.h \
		$(HOME)/lib.bin/em_data.a $(HOME)/h/ip_spec.h
		$(CC) -I$(HOME)/h -O -o mktables mktables.c $(HOME)/lib.bin/em_data.a

em.out:         em.p
		apc -mint -O em.p >emerrs ; mv e.out em.out

em:             em.p
		apc -O -i em.p >emerrs ; mv a.out em

nem.p:          em.p
		sed -e '/maxadr  = t16/s//maxadr  =t15/' -e '/maxdata = 8191; /s//maxdata = 14335;/' -e '/ adr=.*long/s// adr=    0..maxadr/' <em.p >nem.p

nem:            nem.p
		apc -O -i nem.p >emerrs ; mv a.out nem

emdmp:          emdmp.c
		$(CC) -I$(HOME)/h -o emdmp -O emdmp.c

cmp:

pr:		
		@pr em.p mktables.c emdmp.c
