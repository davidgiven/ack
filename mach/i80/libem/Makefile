install:	tail
		../../install tail tail_em

cmp:		tail
		-../../compare tail tail_em

tail:
		arch cr `cat LIST`

opr:
		make pr | opr
pr:
		@pr `pwd`/Makefile
		@pr -l33 `tail +1 LIST|sort`
