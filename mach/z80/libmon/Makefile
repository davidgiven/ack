TAIL=tail.hermac
# Other possibilities are: tail.nascom and tail.cpm

all:		tail.cpm tail.nascom tail.hermac

install:	$(TAIL)
		../../install head_em.s head_em
		../../install $(TAIL) tail_sys

cmp:		$(TAIL)
		-../../compare head_em.s head_em
		-../../compare $(TAIL) tail_sys

tail.cpm:	mon.cpm.s
		@echo Warning: untested, this is an example
		arch cr tail.cpm mon.cpm.s

tail.nascom:	mon.s putchr.nas.s
		arch cr tail.nascom mon.s putchr.nas.s

tail.hermac:	mon.s char.her.s
		arch cr tail.hermac mon.s char.her.s

opr:
		make pr | opr
pr:
		@pr `pwd`/Makefile `pwd`/head_em.s
		@pr `pwd`/subr.s `pwd`/mon.s `pwd`/mon.cpm.s `pwd`/putchr.s `pwd`/putchr.nas.s `pwd`/char.her.s
