.define strhp
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

strhp:
	popl	RR2, *RR14
	pop	R0, *RR14
	ld	reghp, R0	!heappointer must be < stackpointer.
	cp	R0, R15
	jp	ULT, *RR2
	push	*RR14, $EHEAP
	jr	fatal
