.define Rmu4

! This subroutine returns the remainder of a fourbyte unsigned
! division.


Rmu4:
	ldy #1
	sty UNSIGN	! its unsigned
	jsr Pop
	stx ARTH
	sta ARTH+1
	jsr Pop
	stx ARTH+2
	sta ARTH+3	! second operand
	jsr Pop
	stx ARTH+4
	sta ARTH+5
	jsr Pop
	stx ARTH+6
	sta ARTH+7	! first operand
	jsr Duv4
	lsr ARTH+11
	ror ARTH+10
	ror ARTH+9
	ror ARTH+8	! shift result one time
    	lda ARTH+11
	ldx ARTH+10
	jsr Push
	lda ARTH+9
	ldx ARTH+8
	jmp Push


