.define Mli2, Mlinp, Mul

! The subroutine Mli2 multiplies two signed integers. The integers
! are popped from the stack.
! The subroutine Mlinp expects the two integer to be in zeropage.
! While the subroutine Mul an unsigned multiply subroutine is.
! For the algoritme see A. S. Tanenbaum
!	Structured Computer Organisation. 1976.


Mli2:
	stx ARTH
	sta ARTH+1
	jsr Pop
	stx ARTH+2
	sta ARTH+3
Mlinp:	ldy #1
	sty UNSIGN	! it's signed
	lda ARTH+1
	bpl 3f		! multiplier negative so:
	ldx ARTH
	jsr Ngi2	! negate multiplier
	stx ARTH
	sta ARTH+1
	ldx ARTH+2
	lda ARTH+3
	jsr Ngi2	! negate multiplicand
	stx ARTH+2
	sta ARTH+3
Mul:
    3:	lda #0	
	sta ARTH+4
	sta ARTH+5
	sta ARTH+6
	sta ARTH+7	! clear accumulator
    	ldy #16
    1:	lda #1h
	bit ARTH
	beq 2f		! multiplying by zero: no addition
	clc
	lda ARTH+6
	adc ARTH+2
	sta ARTH+6
	lda ARTH+7
	adc ARTH+3
	sta ARTH+7
    2:	lsr ARTH+1
	ror ARTH	! shift multiplier
	lsr ARTH+7
	ror ARTH+6
	ror ARTH+5
	ror ARTH+4	! shift accumulator
	lda UNSIGN
	beq 3f		! unsigned multiply: so no shift in of signbit
	lda ARTH+3
	bpl 3f
	lda #40h
	bit ARTH+7
	beq 3f
	lda ARTH+7
	ora #80h
	sta ARTH+7
    3:	dey
	bne 1b
	ldx ARTH+4
	lda ARTH+5
	rts


