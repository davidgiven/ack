.define Rol

! This subroutine rotates left an integer n times
! N is in register X.
! The result is returned in registerpair AX.


Rol:
	
    	txa
	bne 1f
	jmp Pop		! zero rotate return input
    1:	tay		! Y contains number of rotates
	jsr Pop
	stx Ytmp	! save lowbyte
    2:	clc
	rol Ytmp	! rotate lowbyte
	rol a		! rotate highbyte
	bcc 1f		! no carry
	inc Ytmp	! put carry in rightmost bit
    1:	dey
	bne 2b
	ldx Ytmp	! store lowbyte in X
	rts


