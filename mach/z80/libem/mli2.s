.define .mli2
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! 16 bit multiply
! parameters:
!   bc: multiplicand
!   de: multiplier
!   hl: result (out)
! multiplier (bc) is left unchanged
! no detection of overflow



.mli2:
	ld hl,0
	ld a,16
0:
	bit 7,d
	jr z,1f
	add hl,bc
1:
	dec a
	jr z,2f
	ex de,hl
	add hl,hl
	ex de,hl
	add hl,hl
	jr 0b
2:
	ret
