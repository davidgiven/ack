.define .cms
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! any size sets
! parameters:
!   hl: size
!   stack: second operand
!	   first operand
!   stack: result (out)



.cms:
	pop ix
	ld b,h
	ld c,l
	add hl,sp
0:
	dec sp
	pop af
	cpi
	jr nz,1f
	ld a,b
	or c
	jr nz,0b
	ld de,0
	jr 2f
1:
	add hl,bc
	ld de,1
2:
	ld sp,hl
	push de
	jp (ix)
