.define sar
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

!R1 contains description address
!R3 contains element number
!base address is on stack
sar:
	popl	saveret, *RR14
	sub	R3, 0(R1)
	ld	R0, 4(R1)		!nr of bytes per element
	mult	RR2, R0
	add	R3, *RR14
	inc	R15, $2
	sra	R0			!nr of words per element
	jr	EQ, 1f
	ldir	*RR2, *RR14, R0
	jr	2f
1:	pop	R1, *RR14
	ldb	*RR2, RL1
2:	ldl	RR2, saveret
	jp	*RR2
