.define Mli4

! This subroutine multiplies two signed fourbyte integers
! For more detail see mli.s
! The only difference is that zeropage locations are twice as big.


Mli4:
	ldy #1
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
	lda ARTH+3
	bpl 3f
	lda #0
	ldx #ARTH
	jsr Ngi4
	lda #0
	ldx #ARTH+4
	jsr Ngi4
    3:	jmp Mul4


