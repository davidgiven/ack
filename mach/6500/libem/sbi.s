.define Sbi2

! This subroutine subtracts two twobyte signed integers
! and returnes the result in registerpair AX.


Sbi2:
	stx ARTH	! save second operand (lowbyte)
	sta ARTH+1	! save second operand (highbyte)
	jsr Pop
	pha
	sec
	txa	 	! get first operand (lowbyte)
	sbc ARTH	! subtract second operand (lowbyte)
	tax
	iny
	pla		! get first operand (highbyte)
	sbc ARTH+1	! subtract second operand (highbyte)
	rts


