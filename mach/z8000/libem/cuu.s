.define cuu

cuu:
	popl	RR2, *RR14
	pop	R0, *RR14
	pop	R1, *RR14
	sub	R0, R1		!expansion in bytes
	jr	LE, 1f
	sra	R0		!expansion in words
	jp	EQ, *RR2
2:	push	*RR14, $0
	djnz	R0, 2b
	jp	*RR2
1:	sub	R15, R0
	jp	*RR2
