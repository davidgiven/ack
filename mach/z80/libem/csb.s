.define .csb
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! this is not a subroutine, but just a
! piece of code that computes the jump-
! address and jumps to it.
! traps if resulting address is zero



.csb:
	pop hl		! pointer to descriptor
	pop de		! case index
	ld c,(hl)	! bc := default offset
	inc hl
	ld b,(hl)
	inc hl
	push bc		! save default on stack
	ld c,(hl)	! bc := #entries
	inc hl
	ld b,(hl)
	inc hl
1:
	! loop, try to find the case index
	! in the descriptor
	ld a,b
	or c
	jr z,noteq	! done, index not found
	ld a,(hl)	! is de=(hl) ?
	inc hl
	cp e
	jr nz,2f	! no
	ld a,(hl)
	inc hl
	cp d
	jr nz,3f	! no
	ld a,(hl)	! yes, get jump address
	inc hl
	ld h,(hl)
	ld l,a
	pop af		! remove default
	jr 4f
2:
	inc hl		! skip high byte of index
3:
	inc hl		! skip jump address
	inc hl
	dec bc
	jr 1b
noteq:
	pop hl		! take default exit
4:
	ld a,l		! jump address is zero?
	or h
	jr z,.trp.z	! yes, trap
	jp (hl)
