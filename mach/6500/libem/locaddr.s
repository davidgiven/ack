.define Locaddr

! This routine gets the address of a local which offset is to big.
! The offset is in registerpair AX.


Locaddr:
	pha		! save A
	txa
	clc
	adc LB		! localbase + offset (lowbyte)
	sta ADDR	! address (lowbyte)
	pla
	adc LB+1	! localbase + offset (highbyte)
	sta ADDR+1	! address (highbyte)
	rts


