.define gto

gto:
	pop	R3, *RR14
	ld	R13, 4(R3)
	ld	R15, 2(R3)
	ld	R3, 0(R3)
	jp	0(R3)
