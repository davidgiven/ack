.define lar

!R1 contains description address
!R3 contains element number
!base address is on stack
lar:
	popl	saveret, *RR14
	sub	R3, 0(R1)
	ld	R0, 4(R1)		!nr of bytes per element
	mult	RR2, R0
	add	R3, *RR14
	add	R3, R0
	sra	R0			!nr of words per element
	jr	EQ, 1f
	dec	R3, $2
	lddr	*RR14, *RR2, R0
	inc	R15, $2
	jr	2f
1:	ldb	RL2, -1(R3)
	ldb	RH2, $0
	ld	*RR14, R2
2:	ldl	RR2, saveret
	jp	*RR2
