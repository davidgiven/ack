.define Dvi2, Div, Duv

! The subroutine Dvi2 performs a signed division.
! Its operands are on the stack.
! The subroutine Div performs also a signed division, ecxept that
! its operand are already in zeropage.
! The subroutine Duv performs a n unsigned division.
! For an explanation of the algoritm used see
!   A. S. Tanenbaum's Structered Computer Organisation. 1976


Dvi2:
	stx ARTH
	sta ARTH+1	! store divisor
	jsr Pop
	stx ARTH+2
	sta ARTH+3	! store dividend
	ldy #1
	sty UNSIGN	! used for result sign
Div:
	ldy #0
	sty SIGN
	lda ARTH+1
	bpl 1f		! if divisor is negative
	ldx ARTH	! make it positive
	jsr Ngi2
	ldy #1
	sty SIGN
	stx ARTH
	sta ARTH+1
    1:	lda ARTH+3
	bpl 1f		! if dividend is negative
	ldx ARTH+2	! make it positive
	jsr Ngi2
	pha
	lda SIGN
	eor #1		! excusive or with sign of divisor
	sta SIGN
	lda #1
	sta NBYTES
	pla
    	stx ARTH+2
	sta ARTH+3
Duv:
    1:	ldy #0
	sty ARTH+4
	sty ARTH+5
	ldy #17
    4:	lda ARTH+5
	cmp ARTH+1
	bcc 1f		! no subtraction
	bne 2f		! divisor goes into dividend
	lda ARTH+4
	cmp ARTH
	bcc 1f		! no subtraction
    2:	sec		! divisor goes into dividend
	lda ARTH+4
	sbc ARTH
	sta ARTH+4
	lda ARTH+5
	sbc ARTH+1
	sta ARTH+5	! subtract divisor from dividend
	sec
	rol ARTH+2	! a subtraction so shift in a 1
	bne 3f
    1:	asl ARTH+2	! no subtraction so shift in a 0
    3:	rol ARTH+3
	rol ARTH+4
	rol ARTH+5	! shift dividend
	dey
	bne 4b
	ldx ARTH+2
	lda ARTH+3
	ldy UNSIGN	! is it an unsigned division
	beq 1f
	ldy SIGN	! is the result negative
	beq 1f
	jsr Ngi2
    1:	rts



