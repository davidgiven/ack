.define cms

cms:
	popl	saveret, *RR14
	ldm	savereg, R4, $10
	ldl	RR2, RR14
	add	R3, R0
	ldl	RR4, RR2
	add	R5, R0
	sra	R0
2:	pop	R1, *RR14
	cp	R1, *RR2
	jr	NE, 1f
	inc	R3, $2
	djnz	R0, 2b
1:	ld	R15, R5
	ldm	R4, savereg, $10
	pushl	*RR14, saveret
	ret
