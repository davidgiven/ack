install:	tail
		../../install head.s head_em
		../../install tail tail_sys

cmp:		tail
		-../../compare head.s head_em
		-../../compare tail tail_sys

tail:
		arch cr `cat LIST`

opr:
		make pr | opr
pr:
		@pr `pwd`/Makefile `pwd`/head.s
		@pr -l33 `tail +1 LIST|sort`
