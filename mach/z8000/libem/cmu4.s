.define cmu4
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

cmu4:
	popl	saveret, *RR14
	popl	RR0, *RR14
	popl	RR2, *RR14
	cpl	RR2, RR0
	ldk	R0, $0
	ldl	RR2, saveret
	jr	ULT, 1f
	jp	EQ, *RR2
	inc	R0
	jp	*RR2
1:	dec	R0
	jp	*RR2
