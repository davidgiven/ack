.define cmi

!size in R0
cmi:
	cp	R0, $2
	jr	NE, 1f
	popl	RR2, *RR14
	pop	R1, *RR14
	pop	R0, *RR14
	sub	R0, R1
	jp	*RR2
1:	cp	R0, $4
	jr	EQ, cmi4
	jr	unknown
