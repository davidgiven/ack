.define Sli2

! This subroutine shifts a signed or unsigned interger to the
! left n times.
! N is in register X.
! The returned value is in registerpair AX.


Sli2:
    	txa
	bne 1f
	jmp Pop		! zero shift, return input
    1:	tay
	jsr Pop		! get integer
	stx Ytmp	! save lowbyte
    2:	asl Ytmp
	rol a		! shift left
	dey
	bne 2b
	ldx Ytmp	! get lowbyte
	rts


