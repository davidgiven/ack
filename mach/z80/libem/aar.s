.define .aar
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
! use .mli2

! 2-byte descriptor elements
! any size array elements
! no range checking
! parameters:
!   stack:  pointer to descriptor
!           index
!	    base address of array
!   stack:  result (out)
! uses .mli2 routine
! side-effect: size of array elements in bc



.aar:
	pop hl		! return address
	pop ix		! pointer to descr.
	ex (sp),hl	! save ret. addr.
			! hl := index
	ld c,(ix+0)	! bc := lower bound
	ld b,(ix+1)
	xor a
	sbc hl,bc	! hl := index-lwb
	ld c,(ix+4)	! bc := size
	ld b,(ix+5)
	ex de,hl	! de := index-lwb
	call .mli2	! hl := bc*de =
			!  size*(index-lwb)
	pop ix		! return address
	pop de		! base
	add hl,de	! addr. of element
	push hl
	jp (ix)		! return
