.define .lar
! use .mli2

! 2-byte descriptor elements
! any size array elements
! parameters:
!    on stack
! uses .mli2
! no range checking
! adapted from .aar and .los



.lar:
	pop hl
	pop ix
	ex (sp),hl
	ld c,(ix+0)
	ld b,(ix+1)
	xor a
	sbc hl,bc
	ld c,(ix+4)
	ld b,(ix+5)
	ex de,hl
	call .mli2
	pop ix
	pop de
	add hl,de	! address of array element
	add hl,bc
	dec hl		! pointer to highest byte of element
	srl b
	rr c
	jr nc,1f
	ld a,c		! skip check to save runtime
	or b
	jr nz,.trp.z	! size was odd but <> 1
	ld c,(hl)
	push bc
	jp (ix)
1:	ld d,(hl)
	dec hl
	ld e,(hl)
	dec hl
	push de
	dec bc
	ld a,b
	or c
	jr nz,1b
	jp (ix)
