.define dvu2

dvu2:
	popl	saveret, *RR14
	pop	R2, *RR14
	pop	R1, *RR14
	test	R2
	jr	MI, 1f
	ldk	R0, $0
	div	RR0, R2
2:	pushl	*RR14, saveret
	ret
1:	cp	R2, R1
	ldk	R1, $0
	jr	UGT, 2b
	inc	R1
	jr	2b
