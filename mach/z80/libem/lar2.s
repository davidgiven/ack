.define .lar2

! special case lar: element size = 2 (statically known)
! parameters:
!   on stack
! adapted from .aar2
! execution time: 144 states



.lar2:
	pop ix
	pop hl
	ld c,(hl)
	inc hl
	ld b,(hl)
	pop hl
	xor a
	sbc hl,bc
	add hl,hl	! size*(index-lwb)
	pop de
	add hl,de	! + base
	ld e,(hl)
	inc hl
	ld d,(hl)
	push de
	jp (ix)
