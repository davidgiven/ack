.define Tle

! This subroutine tests if the value in registerpair AX is
! less than or equal to zero.
! The value returned is in AX.


Tle:
	tay
	bpl 1f		! A >= 0
    3:	lda #0		! AX <= 0
	ldx #1
	rts
    1:	beq 1f		! A = 0
    2:	lda #0		! AX > 0
	tax
	rts
    1:	txa
	bne 2b		! X > 0
	beq 3b		! x = 0


