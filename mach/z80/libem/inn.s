.define .inn
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text
! use .unimpld

! any size sets
! parameters:
!   hl:    size
!   stack: bit number
!   stack: result (out)



.inn:
	pop ix
	pop de
	add hl,sp
	ld b,h
	ld c,l
	ex de,hl
	ld a,l
	sra h
	jp m,0f
	rr l
	sra h
	rr l
	sra h
	rr l
	add hl,sp
	push hl
	or a		! clear carry
	sbc hl,de
	pop hl
	jp m,1f
0:	xor a
	jr 4f
1:	ld e,(hl)
	and 7
	jr 2f
3:	rrc e
	dec a
2:	jr nz,3b
	ld a,e
	and 1
4:
	ld e,a
	ld d,0
	ld h,b
	ld l,c
	ld sp,hl
	push de
	jp (ix)
