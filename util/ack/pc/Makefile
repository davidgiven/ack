d=../../..
h=$d/h

PC_PATH=$d/lib/em_pc

em_pc:          em_pc.c $h/local.h $h/em_path.h
		cc -n -o em_pc -O -I$h em_pc.c

cmp:            em_pc
		cmp em_pc $(PC_PATH)

install:        em_pc
		cp em_pc $(PC_PATH)

lint:
		lint -hpxc -I$h em_pc.c

clean:
		rm -f *.o *.old

opr:
		make pr ^ opr

pr:
		pr -n em_pc.c
