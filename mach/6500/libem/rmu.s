.define Rmu2

! This subroutine returns the remainder of an twobyte unsigned
! integer division.


Rmu2:
	stx ARTH
	sta ARTH+1	! first operand
	jsr Pop
	stx ARTH+2
	sta ARTH+3	! second operand
	ldy #1
	sty UNSIGN	! it unsigned
	jsr Duv
	lsr ARTH+5
	ror ARTH+4	! shift result one time
	ldx ARTH+4
	lda ARTH+5
    	rts


