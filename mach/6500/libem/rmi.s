.define Rmi2
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! This subroutine returns the remainder of a twobyte signed division.
! The sign of the result is as specified in the emtest.


Rmi2:
	ldy #0
	sty NBYTES	! for the sign of the result
	stx ARTH
	sta ARTH+1	! first operand
	jsr Pop
	stx ARTH+2
	sta ARTH+3	! second operand
	ldy #0
	sty UNSIGN	! its signed arithmetic
	jsr Div
	lsr ARTH+5
	ror ARTH+4	! result must be shifted one time
	ldx ARTH+4
	lda ARTH+5
	ldy NBYTES
	beq 1f		! result must be positive
	jmp Ngi2
    1:	rts


