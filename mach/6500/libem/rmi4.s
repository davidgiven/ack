.define Rmi4
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! This subroutine returns the remainder of a fourbyte division.


Rmi4:
	ldy #0
	sty NBYTES	! for the sign of the result
	ldy #0
	sty UNSIGN	! it is signed arithmetic
	jsr Div4
	lsr ARTH+11
	ror ARTH+10
	ror ARTH+9
	ror ARTH+8	! result must be shifted one time
	ldy NBYTES
	beq 1f		! result is positive
	lda #0
	ldx #ARTH+8
	jsr Ngi4
    1:	lda ARTH+11
	ldx ARTH+10
	jsr Push
	lda ARTH+9
	ldx ARTH+8
	jmp Push


