.define Test2

! This subroutine tests if the value on top of the stack is 2.
! It is used if the size is on top of the stack.
! The word which is to be handled is returned in registerpair AX.


Test2:
	tay
	bne 1f		! value > 255
	cpx #2
	bne 1f		! value <> 2
	jsr Pop		! get word
	rts
    1:	ldx #Eoddz
	lda #0
	jsr Trap


