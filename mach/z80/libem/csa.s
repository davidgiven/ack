.define .csa

! this is not a subroutine, but just a
! piece of code that computes the jump-
! address and jumps to it.
! traps if resulting address is zero



.csa:
	pop ix
	pop hl
	push bc
	ld c,(ix)
	ld b,(ix+1)
	ld e,(ix+2)
	ld d,(ix+3)
	xor a
	sbc hl,de
	jp m,1f
	ex de,hl
	ld l,(ix+4)
	ld h,(ix+5)
	xor a
	sbc hl,de
	jp m,1f
	ex de,hl
	add hl,hl
	ld de,6
	add hl,de
	ex de,hl
	add ix,de
	ld l,(ix)
	ld h,(ix+1)
	ld a,h
	or l
	jr nz,2f
1:	ld a,b
	or c
	jr z,.trp.z
	ld l,c
	ld h,b
2:	pop bc
	jp (hl)
