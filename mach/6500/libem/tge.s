.define Tge
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! This subroutine test if the value in registerpair AX is
! greater than or equal to zero.
! The result is returned in AX.


Tge:
	tay
	bpl 1f		! A >= 0
	lda #0		! AX < 0
	tax
	rts
    1:	lda #0		! AX >= 0
	ldx #1
	rts


