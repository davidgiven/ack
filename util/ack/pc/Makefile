EMHOME=../../..
h=$(EMHOME)/h

PC_PATH=$(EMHOME)/lib/em_pc

LDFLAGS = -i

em_pc:          em_pc.c $h/local.h $h/em_path.h
		cc $(LDFLAGS) -o em_pc -O -I$h em_pc.c

cmp:            em_pc
		cmp em_pc $(PC_PATH)

install:        em_pc
		rm -f $(PC_PATH)
		cp em_pc $(PC_PATH)

lint:
		lint -hpxc -I$h em_pc.c

clean:
		rm -f *.o *.old em_pc

opr:
		make pr ^ opr

pr:
		pr -n em_pc.c
