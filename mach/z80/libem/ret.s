.define .ret
.define .lfr

.sect .text
.sect .rom
.sect .data
.sect .bss

! 'ret' and 'lfr' for sizes > 4 (and <= 8)

retarea: .space 8

.sect .text

.ret:
! parameters:
!	de: size in bytes
!	stack: return value

	pop ix		! save return address
	ld hl,retarea
	srl d
	rr e		! divide size by 2
1:
	pop bc
	ld (hl), c
	inc hl
	ld (hl), b
	inc hl
	dec de
	ld a,d
	or e
	jr nz,1b	! done?
	jp (ix)		! yes, return

.lfr:
! parameters:
!	de: size in bytes
!	result on stack: return value

	pop ix		! save return address
	ld hl,retarea
	add hl,de
	srl d
	rr e		! divide size by 2
1:
	dec hl
	ld b,(hl)
	dec hl
	ld c,(hl)
	push bc
	dec de
	ld a,d
	or e
	jr nz,1b	! done?
	jp (ix)		! yes, return

