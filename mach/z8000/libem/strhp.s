.define strhp
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

strhp:
	popl	RR2, *RR14
	pop	R0, *RR14
	cp	R0, R15
	jp	ULT, 1f
	push	*RR14, $EHEAP
	calr	trp
	jp	*RR2
1:
	ld	reghp, R0
	jp	*RR2
