PC_TAIL=tail_pc.a

head:
	echo This Makefile needs arguments

clean:
	rm -f *.old

opr:
	make pr | opr

pr:
	@pr `echo * | sed s/$(PC_TAIL)//`
	@ar pv $(PC_TAIL) | pr -h $(PC_TAIL)
