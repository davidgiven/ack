.define Dvu4

! This subroutine performs an unsigned division on fourbyte
! integers. For more details see dvi.s
! The only difference is that zeropage locations are twice as big.


Dvu4:
	ldy #0
	sty UNSIGN	! it is unsigned
	jsr Pop
	stx ARTH
	sta ARTH+1
	jsr Pop
	stx ARTH+2
	sta ARTH+3	! divisor in ARTH - ARTH+3
	jsr Pop
	stx ARTH+4
	sta ARTH+5
	jsr Pop
	stx ARTH+6
	sta ARTH+7	! dividend in ARTH+4 - ARTH+7
	jsr Duv4
	lda ARTH+7
	ldx ARTH+6
	jsr Push
	lda ARTH+5
	ldx ARTH+4	 
	jmp Push	! store result


