.define Lcs
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! This subroutine creates space for locals on procedure entry
! by lowering the stackpointer.


Lcs:
	sta ARTH	! number of locals (lowbyte)
	stx ARTH+1	! number of locals (highbyte)
	sec
	lda SP+2
	sbc ARTH
	sta SP+2	! new stackpointer (lowbyte)
	lda SP+1
	sbc ARTH+1
	sta SP+1	! new stackpointer (highbyte)
	rts


