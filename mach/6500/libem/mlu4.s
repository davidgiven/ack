.define Mlu4

! This subroutine multiplies two fourbyte unsigned integers.
! For more details see mli.s
! The only difference is that zeropage locations are twice as big.


Mlu4:
	ldy #0
	sty UNSIGN
	jsr Pop
	stx ARTH
	sta ARTH+1
	jsr Pop
	stx ARTH+2
	sta ARTH+3	! multiplier
	jsr Pop
	stx ARTH+4
	sta ARTH+5
	jsr Pop
	stx ARTH+6
	sta ARTH+7	! multiplicand
	jmp Mul4


