install:
	../../install head_em
	../../install tail_mon

cmp:
	-../../compare head_em
	-../../compare tail_mon

clean :

opr :
	make pr | opr

pr:
	@pr `pwd`/head_em
	@arch pv tail_mon | pr -h `pwd`/tail_mon
