.define Ror

! This subroutine rotates right a integer twobyte word.
! The number of rotates is in X.
! The result is returned in registerpair AX.


Ror:
    	txa
	bne 1f		! a zero rotate just return input
	jmp Pop
    1:	tay
	jsr Pop		! get word
	stx Ytmp	! save lowbyte
    2:	clc
	ror a		! rotate highbyte
	ror Ytmp	! rotate lowbyte
	bcc 1f		! no carry
	ora #80h	! put carry in leftmost bit
    1:	dey
	bne 2b
	ldx Ytmp	! get lowbyte
	rts


