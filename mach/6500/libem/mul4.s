.define Mul4
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! This subroutine multiplies two fourbyte signed integers.
! For more details see mli.s
! The only difference is that zeropage locations are twice as big.


Mul4:
    3:	lda #0
	sta ARTH+8
	sta ARTH+9
	sta ARTH+10
	sta ARTH+11
	sta ARTH+12
	sta ARTH+13
	sta ARTH+14
	sta ARTH+15	! clear accumulator
    	ldy #32
    1:	lda #0x01
	bit ARTH
	beq 2f		! multiplying by zero: no addition
	clc
	lda ARTH+12
	adc ARTH+4
	sta ARTH+12
	lda ARTH+13
	adc ARTH+5
	sta ARTH+13
	lda ARTH+14
	adc ARTH+6
	sta ARTH+14
	lda ARTH+15
	adc ARTH+7
	sta ARTH+15
    2:	lsr ARTH+3
	ror ARTH+2
	ror ARTH+1
	ror ARTH	! shift multiplier
	lsr ARTH+15
	ror ARTH+14
	ror ARTH+13
	ror ARTH+12
	ror ARTH+11
	ror ARTH+10
	ror ARTH+9
	ror ARTH+8	! shift accumulator
	lda UNSIGN
	beq 3f		! it's unsigned: so no shift in of signbit
	lda ARTH+7
	bpl 3f
	lda #0x40
	bit ARTH+15
	beq 3f
	lda ARTH+15
	ora #0x80
	sta ARTH+15
    3:	dey
	bne 1b
	ldx ARTH+10
	lda ARTH+11
	jsr Push
	ldx ARTH+8
	lda ARTH+9
	jmp Push


