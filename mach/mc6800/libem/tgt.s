.define Tgt
.sect .zero
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! This subroutine tests if the value in registerpair AB is
! greater than zero.
! The value returned is in AB.

Tgt:
	tsta
	bpl 1f		! A >= 0
    3:	clrb		! AB <= 0
	clra
	rts
    1:	beq 1f		! A == 0
    2:	ldab #1		! AB > 0
	clra
	rts
    1:	tstb
	bne 2b		! AB > 0
	beq 3b		! AB = 0


