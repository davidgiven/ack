.define .sdl

! store double local at any offset
! parameters:
!    hl: offset
!    stack: operand (4 bytes)



.sdl:
	pop ix		! return address
	push iy		! bc := LB
	pop bc
	add hl,bc	! pointer to lowest byte
			! of local
	pop bc		! low 2 bytes of source
	ld (hl),c
	inc hl
	ld (hl),b
	inc hl
	pop bc		! high 2 bytes of source
	ld (hl),c
	inc hl
	ld (hl),b
	jp (ix)		! return

