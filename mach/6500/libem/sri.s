.define Sri2, Sru2

! The subroutine Sri2 shifts a signed integer n times right.
! In the case of a negative integer there is signextension.
! The subroutine Sru2 shifts right an unsigned integer.
! The returned value is in registerpair AX.


Sru2:
	txa
	bne 1f
	jmp Pop		! zero shift, return input
    1:	tay
	jsr Pop		! get integer
	stx Ytmp	! save lowbyte
	jmp 2f		! shift unsigned
Sri2:
	txa
	bne 1f
	jmp Pop		! zero shift, return input
    1:	tay
	jsr Pop		! get integer
	stx Ytmp	! save lowbyte
	tax
	bmi 1f		! negative signextended shift
    2:	lsr a
	ror Ytmp	! shift not signextended
	dey
	bne 2b
	ldx Ytmp	! get lowbyte
	rts
    1:	sec		! shift signextended
	ror a
	ror Ytmp
	dey
	bne 1b
	ldx Ytmp	! get lowbyte
	rts


