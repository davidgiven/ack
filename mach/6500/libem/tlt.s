.define Tlt
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! This subroutine tests if the value in registerpair AX is
! less than zero.
! The value returned is in AX.


Tlt:
	tay
	bpl 1f		! A >= 0
	lda #0		! AX < 0
	ldx #1
	rts
    1:	lda #0		! AX >= 0
	tax
	rts


