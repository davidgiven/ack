.define sts2

sts2:
	popl	saveret, *RR14
	pop	R0, *RR14		!object size
	ldk	R2, $0
	pop	R3, *RR14		!address of object
	cp	R0, $1
	jr	NE, 1f
	pop	R0, *RR14
	ldb	*RR2, RL0
	jr	2f
1:	sra	R0
	ldir	*RR2, *RR14, R0
2:	pushl	*RR14, saveret
	ret
