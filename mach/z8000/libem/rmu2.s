.define rmu2
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

rmu2:
	popl	saveret, *RR14
	pop	R2, *RR14
	pop	R1, *RR14
	test	R2
	jr	MI, 1f
	ldk	R0, $0
	div	RR0, R2
2:	pushl	*RR14, saveret
	ret
1:	ld	R0, R1
	cp	R2, R1
	jp	UGT, 2b
	sub	R0, R2
	jp	2b
