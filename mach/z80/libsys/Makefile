TAIL=libhermac_o.a
# Other possibilities are: tail.nascom and tail.cpm
MACH=z80

all:		head_em.o libhermac_o.a libnascom_o.a libcpm_o.a

install:	head_em.o $(TAIL)
		../../install head_em.o head_em
		../../install $(TAIL) tail_mon

cmp:		head_em.o $(TAIL)
		-../../compare head_em.o head_em
		-../../compare $(TAIL) tail_mon

head_em.o:	head_em.s
		$(MACH) -I../../../h -c head_em.s

libcpm_o.a:	libcpm_s.a
		ASAR=aal ; export ASAR; march . libcpm_o.a

libnascom_o.a:	libnascom_s.a
		ASAR=aal ; export ASAR; march . libnascom_o.a

libhermac_o.a:	libhermac_s.a
		ASAR=aal ; export ASAR; march . libhermac_o.a

libcpm_s.a:
		@echo Warning: untested, this is an example
		arch cr libcpm_s.a mon.cpm.s
		echo libcpm_s.a > LIST
		arch t libcpm_s.a >> LIST

libnascom_s.a:
		arch cr libnascom_s.a mon.s char.nas.s
		echo libnascom_s.a > LIST
		arch t libnascom_s.a >> LIST

libhermac_s.a:
		arch cr libhermac_s.a mon.s char.her.s
		echo libhermac_s.a > LIST
		arch t libhermac_s.a >> LIST

opr:
		make pr | opr
pr:
		@pr `pwd`/Makefile `pwd`/head_em.s
		@pr `pwd`/mon.s `pwd`/mon.cpm.s `pwd`/char.nas.s `pwd`/char.her.s
