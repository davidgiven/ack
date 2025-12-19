.define Tle
.sect .zero
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! This subroutine tests if the value in registerpair AB is
! less than or equal to zero.
! The value returned is in AB.


Tle:
	tsta
	bpl 1f		! A >= 0
    3:	ldab #1		! AB <= 0
	clra
	rts
    1:	beq 1f		! A = 0
    2:	clrb		! AB > 0
	clra
	rts
    1:	tstb
	bne 2b		! B > 0
	beq 3b		! B = 0


