.define .and
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! auxiliary size 'and'
! parameters:
!    de: size
!    stack: operands
!    stack: result (out)



.and:
	pop ix		! save return address
	ld h,d
	ld l,e
	add hl,sp
	ex de,hl
	add hl,de	! now hl is the base of second
	ld b,d		! operand.  bc and de are base
	ld c,e		! of the first operand
1:
	dec hl
	dec de
	ld a,(de)
	and (hl)
	ld (hl),a
	xor a
	sbc hl,bc
	jr z,2f
	add hl,bc
	jr 1b
2:
	ld h,b
	ld l,c
	ld sp,hl
	jp (ix)
