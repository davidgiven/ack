.define .cii
.define .cuu
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! cii: convert integer to integer
! parameters:
!    stack:   destination size
!	      source size
!	      source
!    stack:   result (out)
! This code is also used by cuu.
! The contents of the a-register determines
! if we're doing a cii (a=0) or a cuu (a=1),
! so be very careful with this register!



.cii:
	pop ix		! return address
	pop hl		! destination size
	pop de		! source size
	ld b,h		! bc := destination size
	ld c,l
	xor a		! watch it, this is dirty!
			! Besides clearing the carry
			! this instruction sets a-reg.
			! to 0, to indicate this is
			! a cii and not a cuu.
	sbc hl,de	! hl := destination size
			!  - source size
	jr z,1f		! equal, return
	jp p,2f		! larger, expand
	! smaller, shrink
	! The most significant part of the source
	! is removed. As the least sign. part is
	! on top of the stack, we have to move an
	! entire data block.
9:
	add hl,sp	! note that hl < 0
			! (also come here via cuu)
	add hl,de
	dec hl		! now hl points to most
			! significant byte of what
			! will be left over of source
	ex de,hl
	add hl,sp
	ex de,hl
	dec de		! now de points to highest
			! byte of source
	lddr		! move 'destination size'
			! bytes upwards (i.e. away
			! from top of stack)
	inc de
	ex de,hl
	ld sp,hl	! adjust stackpointer
1:
	jp (ix)		! return

2:
	! larger, expand
	! A number of bytes (containing the signbits
	! of the source) is inserted before the most
	! significant byte of the source.
	! As this byte is somewhere in the middle of
	! the stack, the entire source must first be
	! moved downwards (in the direction of the
	! top)
8:
	ld b,d		! bc := source size
			! (also come here via cuu)
	ld c,e
	ex de,hl	! de := difference (> 0)
	ld hl,0
	add hl,sp	! hl := sp
	dec de		! if difference = 1, don't adjust stack pointer
	jr nz, 4f
	inc de
	jr 5f
4:
	inc de
	push hl
	or a
	sbc hl,de
	ex de,hl	! de := sp - difference
	pop hl		! hl := sp
	ex de,hl	! adjust sp
	ld sp,hl
	ex de,hl
	ldir		! move source upwards,
			! creating a 'hole'
			! inside the stack
	! now we will fill the hole with bytes
	! containing either 0 or -1, depending
	! on the signbit of the source.
	or a
	sbc hl,de
	ex de,hl	! de := difference
	dec hl		! now hl points to
			! most significant byte
			! of the source
5:
	or a		! see if we're doing
			! a 'cii' or a 'cuu'
	jr nz,3f	! cuu, expand with zeroes
	bit 7,(hl)	! test signbit
	jr z,3f
	dec b		! b := -1 (was 0 after ldir)
3:
	inc hl
	ld (hl),b	! either 0 or -1
	dec de
	ld a,d
	or e
	jr nz,3b
	jp (ix)		! return




! cuu: convert unsigned to unsigned
! parameters:
!    stack:  destination size
!	     source size
!	     source
!    stack:  result (out)
! The only difference between a cuu and a cii is:
! if the destination is larger than the source,
! the former extends with zeroes and the latter
! extends with sign bits
! cuu uses the code of cii. In this case it puts
! a '1' in the accumulator to indicate this is
! a cuu.



.cuu:
	pop ix
	pop hl
	pop de
	ld b,h
	ld c,l
	xor a		! clear carry
	sbc hl,de
	jr z,1b		! equal, return
	jp m,9b		! smaller, shrink
	inc a		! a := 1
	jr 8b		! larger, expand
