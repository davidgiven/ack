.define Ngi2
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! This subroutine negates the integer in registerpair AX.
! The negation is a one's complement plus one.


Ngi2:
	eor #0FFh	! one's complement A
	tay
	txa
	eor #0FFh	! one's complement X
	tax
	inx		! increment X
	bne 1f
	iny		! increment A if neccesairy
    1:	tya
	rts


