d=../..
l=$d/lib
h=$d/h
ASS_PATH=$l/em_ass

SEP_OPT=-i

CFLAGS=-O

all:            ass$(SEP_OPT)

clean:
		-rm -f ass-i ass-n *.o maktab *.old asstb.c asprint

install :       all
		cp ass$(SEP_OPT) $(ASS_PATH)

cmp :           all
		cmp ass$(SEP_OPT) $(ASS_PATH)

lint:           ass00.c ass30.c ass40.c ass50.c ass60.c ass70.c \
			ass80.c assci.c assda.c assrl.c asstb.c asscm.c
		lint -hpvbx \
			ass00.c ass30.c ass40.c ass50.c ass60.c ass70.c \
			ass80.c assci.c assda.c assrl.c asstb.c asscm.c


ass-n:       ass00.o ass30.o ass40.o ass50.o ass60.o ass70.o \
			ass80.o assci.o assda.o assrl.o asstb.o asscm.o \
			$l/em_data.a
		cc -n $(CFLAGS) -o ass-n \
			ass00.o ass30.o ass40.o ass50.o ass60.o ass70.o \
			ass80.o assci.o assda.o assrl.o asstb.o asscm.o \
			$l/em_data.a

ass-i:       ass00.o ass30.o ass40.o ass50.o ass60.o ass70.o \
			ass80.o assci.o assda.o assrl.o asstb.o asscm.o \
			$l/em_data.a
		cc -i $(CFLAGS) -o ass-i \
			ass00.o ass30.o ass40.o ass50.o ass60.o ass70.o \
			ass80.o assci.o assda.o assrl.o asstb.o asscm.o \
			$l/em_data.a

ass00.o ass40.o ass60.o ass70.o ass80.o assrl.o: \
		$h/local.h $h/em_spec.h $h/as_spec.h \
		$h/em_flag.h $h/arch.h ass00.h assex.h

assci.o:        $h/local.h $h/em_spec.h $h/as_spec.h \
		$h/em_flag.h $h/em_mes.h $h/em_pseu.h \
		$h/em_ptyp.h $h/arch.h ass00.h assex.h

ass30.o ass50.o : \
		$h/local.h $h/em_spec.h $h/as_spec.h \
		$h/em_flag.h ip_spec.h ass00.h assex.h

ass80.o:        $h/em_path.h

assda.o:        $h/local.h $h/em_spec.h $h/as_spec.h \
		$h/em_flag.h $h/arch.h ass00.h

asscm.o:        ass00.h

asstb.o:        asstb.c

asstb.c:        maktab ip_spec.t
		maktab ip_spec.t asstb.c

maktab:         maktab.c $h/em_spec.h ip_spec.h $h/em_flag.h \
		$l/em_data.a
		cc -O -o maktab maktab.c $l/em_data.a

asprint:	asprint.p
		apc -w -o asprint asprint.p

opr:
		make pr ^ opr

pr:
		@(pr ass00.h assex.h ip_spec.h ass?0.c ass[rcd]?.c \
		maktab.c ; pr -3 ip_spec.t)
