.define aar

!R1 contains description address
!R3 contains element number
!base address is on stack
aar:
	sub	R3, 0(R1)
	mult	RR2, 4(R1)
	inc	R15, $4
	add	R3, *RR14
	ld	*RR14, R3
	dec	R15, $4
	ret
