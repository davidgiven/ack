.define .rmi2

! 16-bit signed remainder
! parameters:
!   bc: divisor
!   de: dividend
!   de: result (out)
! no check on overflow



.rmi2:
	xor	a
	ld	h,a
	ld	l,a
	sbc	hl,bc
	jp	m,1f
	ld	b,h
	ld	c,l
1:
	or	a
	ld	hl,0
	sbc	hl,de
	jp	m,1f
	ex	de,hl
	cpl
1:
	push	af
	ld	hl,0
	ld	a,16
0:
	add	hl,hl
	ex	de,hl
	add	hl,hl
	ex	de,hl
	jr	nc,1f
	inc	hl
	or	a
1:
	sbc	hl,bc
	inc	de
	jp	p,2f
	add	hl,bc
	dec	de
2:
	dec	a
	jr	nz,0b
	ex	de,hl
	pop	af
	or	a
	jr	z,1f
	ld	hl,0
	sbc	hl,de
	ex	de,hl
1:
	ret
