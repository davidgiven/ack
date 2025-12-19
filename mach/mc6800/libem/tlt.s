.define Tlt
.sect .zero
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! This subroutine tests if the value in registerpair AX is
! less than zero.
! The value returned is in AX.


Tlt:
	clrb
	asla
	rolb
	clra
	rts
