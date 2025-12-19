.define Sbi2
.sect .zero
.sect .text
.sect .rom
.sect .data
.sect .bss

! This subroutine subtracts two twobyte signed integers
! and returnes the result in registerpair AB.


.sect .text
Sbi2:
	tsx
	ldx 0,x
	ins
	ins
	stab ARTH+1	! save second operand (highbyte)
	staa ARTH	! save second operand (lowbyte)
	pula
	pulb
	subb ARTH+1	! subtract second operand (lowbyte)
	sbca ARTH	! subtract second operand (highbyte)
	jmp 0,x
