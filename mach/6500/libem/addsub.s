.define Addsub

! This subroutine is used by the fourbyte addition and subtraction
! routines.
! It puts the address of the second operand into
! the zeropage locations ADDR and ADDR+1
! The address of the first operand is put into
! zeropage locations ADDR+2 and ADDR+3.


Addsub:
	clc
	lda SP+2
	sta ADDR	! address of second operand (lowbyte)
	adc #4
	sta SP+2
	sta ADDR+2	! address of first operand (lowbyte)
	lda SP+1
	sta ADDR+1	! address of second operand (highbyte)
	adc #0
	sta ADDR+3	! address of first operand (highbyte)
	sta SP+1
	ldy #0
	ldx #0FCh	! do it 4 times
	rts


