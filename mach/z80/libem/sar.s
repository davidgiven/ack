.define .sar
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
! use .mli2
! use .trp.z

! 2-byte descriptors
! any size array elements
! parameters:
!    on stack
! uses .mli2
! adapted from .aar and .sts



.sar:
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
	add hl,de
	srl b		! bc contains #bytes to transfer
	rr c		! divide bc by 2
	jr nc,1f
	ld a,c
	or b
	jr nz,.trp.z
	pop bc
	ld (hl),c
	jp (ix)
1:
	pop de
	ld (hl),e
	inc hl
	ld (hl),d
	inc hl
	dec bc
	ld a,b
	or c
	jr nz,1b
	jp (ix)
