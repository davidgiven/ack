.define cmu
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

!size in R0
cmu:
	cp	R0, $2
	jr	NE, 1f
	popl	RR2, *RR14
	pop	R1, *RR14
	pop	R0, *RR14
	cp	R0, R1
	ldk	R0, $0
	jr	ULT, 2f
	jp	EQ, *RR2
	inc	R0
	jp	*RR2
2:	dec	R0
	jp	*RR2
1:	cp	R0, $4
	jr	EQ, cmu4
	jr	unknown
