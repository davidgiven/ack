.define csa
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

!R1 contains address of jump table
!R2 contains case index
csa:
	sub	R2, 2(R1)
	jr	LT, 1f
	cp	R2, 4(R1)
	jr	UGT, 1f
	sla	R2
	add	R1, R2
	ld	R2, 06(R1)
	cp	R2, $0
	jr	EQ, 2f
	jp	0(R2)
1:	ld	R1, 0(R1)
	jp	NE, 0(R1)
2:	push	*RR14, $ECASE
	jr	fatal
