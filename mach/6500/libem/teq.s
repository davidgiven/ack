.define Teq

! This subroutine test if the value in registerpair AX is zero
! or nonzero.
! The returned value, a 1 or a 0, is in AX.


Teq:
	tay
	beq 1f		! A is zero
    2:	lda #0		! AX is zero
	tax
	rts
    1:	txa
	bne 2b		! X is zero
	lda #0		! AX is nonzero
	ldx #1
	rts


