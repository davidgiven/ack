.define .sts
! use trp.z

! object size given by 2-byte integer on
! top of stack.
! parameters:
!   on stack
! checks if #bytes is even or 1,
! else traps



.sts:
	pop ix		! save return address
	pop de		! # bytes to transfer
	pop hl		! destination address
	srl d		! divide de by 2
	rr e
	jr nc,1f	! see if it was odd
	ld a,e		! yes, must be 1
	or d
	jr nz,.trp.z	! no, error
	pop de		! transfer 1 byte,
			! padded with zeroes
	ld (hl),e
	jp (ix)
1:
	pop bc
	ld (hl), c
	inc hl
	ld (hl),b
	inc hl
	dec de
	ld a,e
	or d
	jr nz,1b
	jp (ix)
