.define blm
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

!size in R0
blm:
	popl	saveret, *RR14
	ldm	savereg, R4, $10
	ldk	R2, $0
	ld	R4, R2
	pop	R3, *RR14		!RR2: dst address
	pop	R5, *RR14		!RR4: src address
	ld	R1, R0
	sra	R0
	jr	EQ, 2f
	    !now avoid wrong copy in case the pieces overlap
	cp	R5, R3
	jr	EQ, 2f
	jr	LT, 1f
	ldir	*RR2, *RR4, R0
	jr	2f
1:	dec	R1, $2
	add	R3, R1
	add	R5, R1
	lddr	*RR2, *RR4, R0
2:	ldm	R4, savereg, $10
	pushl	*RR14, saveret
	ret
