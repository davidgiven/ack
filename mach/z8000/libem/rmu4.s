.define rmu4
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

rmu4:
	popl	saveret, *RR14
	ldm	savereg, R4, $10
	popl	RR4, *RR14
	popl	RR2, *RR14
	testl	RR4
	jr	MI, 1f
	ldl	RR0, $0
	divl	RQ0, RR4
	jr	2f
1:	ldl	RR0, RR2
	cpl	RR4, RR2
	jr	UGT, 2f
	sub	RR0, RR4
2:	ldm	R4, savereg, $10
	pushl	*RR14, saveret
	ret
