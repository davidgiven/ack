.define .aar2
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! special case aar: element size = 2 (statically known)
! parameters:
!    on stack
! execution time: 124 states



.aar2:
	pop ix		! save return address
	pop hl		! pointer to descriptor
	ld c,(hl)	! bc := lower bound
	inc hl
	ld b,(hl)
	pop hl		! index
	xor a
	sbc hl,bc	! index - lwb
	add hl,hl	! size*(index-lwb)
	pop de		! base address of array
	add hl,de
	push hl
	jp (ix)
