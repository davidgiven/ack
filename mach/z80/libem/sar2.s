.define .sar2

! special case sar: element size = 2 (statically known)
! parameters:
!   on stack
! adapted from .aar2
! execution time: 143 states



.sar2:
	pop ix
	pop hl
	ld c,(hl)
	inc hl
	ld b,(hl)
	pop hl
	xor a
	sbc hl,bc
	add hl,hl
	pop de
	add hl,de
	pop de
	ld (hl),e
	inc hl
	ld (hl),d
	jp (ix)
