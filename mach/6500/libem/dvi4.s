.define Dvi4
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! This subroutine performs a fourbyte signed division.
! For more details see dvi.s
! The only difference is that zeropage locations are twice as big.


Dvi4:
	ldy #1
	sty UNSIGN
	jsr Div4
	lda ARTH+7
	ldx ARTH+6
	jsr Push
	lda ARTH+5
	ldx ARTH+4
	jmp Push


