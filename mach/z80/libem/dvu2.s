.define .dvu2

! 16-bit divide
! parameters:
!    bc: divisor
!    de: dividend
!    de: quotient (out)
!    hl: remainder (out)
! no overflow detection



.dvu2:
	or a
	ld h,d
	ld l,e
	sbc hl,bc
	jp m,3f
	jp c,3f	! bc > de?
	ld hl,0
	ld a,16
0:
	add hl,hl
	ex de,hl
	add hl,hl
	ex de,hl
	jr nc,1f
	inc hl
	or a
1:
	sbc hl,bc
	inc de
	jp p,2f
	add hl,bc
	dec de
2:
	dec a
	jr nz,0b
	ret
3:
	ld hl,0
	ex de,hl
	ret
