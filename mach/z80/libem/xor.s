.define .xor

! auxiliary size 'xor'
! parameters:
!    de: size
!    stack: operands
!    stack: result (out)



.xor:
	pop ix
	ld h,d
	ld l,e
	add hl,sp
	ld b,h
	ld c,l
	ex de,hl
	add hl,de
1:	dec hl
	dec de
	ld a,(de)
	xor (hl)
	ld (hl),a
	xor a
	sbc hl,bc
	jr z,2f
	add hl,bc
	jr 1b
2:	ld h,b
	ld l,c
	ld sp,hl
	jp (ix)
