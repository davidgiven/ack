all:
	cp head.s em_head
	../../install em_head
	rm -f em_head
	cp e.a em_tail
	-../../compare em_tail
	rm -f em_head em_tail
