.define .trp.z
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! changed into output routine to print errornumber

.trp.z:
!	exx
	pop bc
	pop hl		!error number
	push hl
	ld de,15
	sbc hl,de
	jp p,1f		! error no >= 16?
	pop hl
	push hl		! save error no on stack
	push bc
	push ix
	push hl		! test bit "error no" of ignmask
	ld hl,(ignmask)
	ex (sp),hl
	push hl
	ld hl,2
	call .inn
	pop hl
	pop ix
	pop bc
	ld a,h
	or l
	jr z,2f			! if bit <> 0 error
1:
	pop hl
	push iy
	push de
	ld iy,1f+6
	call outdec
	ld iy,13
	push iy
	ld iy,1f
	push iy
	ld iy,2
	push iy
	call __write
	pop iy
	pop iy
	pop iy
	pop de
	pop iy
	push de
	call __exit
2:
	pop hl
	push bc
!	exx
	ret
1:	.asciz 'error xxxxx\r\n'

