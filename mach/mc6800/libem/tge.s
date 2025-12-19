.define Tge
.sect .zero
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! This subroutine test if the value in registerpair AB is
! greater than or equal to zero.
! The result is returned in AB.


Tge:
	clrb
	eora #0x80
	asla
	rolb
	clra
	rts
