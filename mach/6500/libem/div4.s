.define Div4
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! This subroutine performs a signed divide on two fourbyte integers.
! For more detail see dvi.s
! The only difference is that zeropage locations are twice as big.

Div4:
	ldy #0
	sty SIGN
	jsr Pop
	stx ARTH
	sta ARTH+1
	jsr Pop
	stx ARTH+2
	sta ARTH+3	! divisor in ARTH - ARTH+3
	tay
	bpl 1f
	lda #0
	ldx #ARTH
	jsr Ngi4
	ldy #1
	sty SIGN	! it's signed
    1:	jsr Pop
	stx ARTH+4
	sta ARTH+5
	jsr Pop
	stx ARTH+6
	sta ARTH+7	! dividend in ARTH+4 - ARTH+7
	tay
	bpl 1f
	lda #0
	ldx #ARTH+4
	jsr Ngi4
	lda SIGN
	eor #1
	sta SIGN
	lda #1
	sta NBYTES
    1:	jmp Duv4


