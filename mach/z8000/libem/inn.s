.define inn
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

!bitnr in R1
!size (bytes) in R2
inn:
	ld	R3, R2
	sra	R2
	ldk	R0, $0
	div	RR0, $020	!R0: bitnr, R1: wordnr
	cp	R1, R2
	jr	UGE, 1f		!R1 must be < R2
	inc	R1, $2		!R1 contains nr of words from top stack
	sla	R1
	ld	R1, RR14(R1)
	bit	R1, R0
	jr	EQ, 1f
	ldk	R0, $1
	jr	2f
1:	ldk	R0, $0
2:	ld	R1, R3
	popl	RR2, *RR14
	add	R15, R1
	jp	*RR2
