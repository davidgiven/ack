.define Tne

! This subroutine tests if the value in registerpair AX is
! not equal to zero.
! The value returned is in AX.


Tne:
	tay
	beq 1f		! A = 0
    2:	lda #0		! AX <> 0
	ldx #1
	rts
    1:	txa
	bne 2b		! X <> 0
	lda #0		! AX = 0
	tax
	rts


