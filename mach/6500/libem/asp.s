.define Asp
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! This subroutine adds an offset to the stackpointer,
! e.g. after the return from a procedurecall.
! The offset is in registerpair AX, and is added to the
! stackpointer.


Asp:
	tay		! save A
	txa		! get X
	clc
	adc SP+2	! add adjustment (lowbyte)
	sta SP+2	! new stackpointer (lowbyte)
	tya		! get A
	adc SP+1	! add adjustment (highbyte)
	sta SP+1	! get stackpointer (highbyte)
	rts


