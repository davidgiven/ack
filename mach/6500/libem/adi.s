.define  Adi2

! This subroutine adds two twobyte integers.
! The first operand is on the top of the stack, the second operand
! is in the AX registerpair.
! The result is returned in registerpair AX.


Adi2:
	sta ARTH+1	! second operand (highbyte)
	stx ARTH	! second operand (lowbyte)
	jsr Pop		! get first operand
	pha		! save A
	clc
	txa
	adc ARTH	! add lowbytes
	tax
	pla		! get A
	adc ARTH+1	! add the highbytes
	rts


