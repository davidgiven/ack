.define Duv4

! This subroutine performs an unsigned division on two fourbyte
! unsigned integers.
! For more details see dvi.s
! The only difference is that zeropage locations are twice as big.


Duv4:
    1:	ldy #0
	sty ARTH+8
	sty ARTH+9
	sty ARTH+10
	sty ARTH+11
    	ldy #33
    4:	lda ARTH+11
	cmp ARTH+3
	bcc 1f		! no sub
	bne 2f		! sub
	lda ARTH+10
	cmp ARTH+2
	bcc 1f
	bne 2f
	lda ARTH+9
	cmp ARTH+1
	bcc 1f
	bne 2f
	lda ARTH+8
	cmp ARTH
	bcc 1f
    2:	sec
	lda ARTH+8
	sbc ARTH
	sta ARTH+8
	lda ARTH+9
	sbc ARTH+1
	sta ARTH+9
	lda ARTH+10
	sbc ARTH+2
	sta ARTH+10
	lda ARTH+11
	sbc ARTH+3
	sta ARTH+11
	sec
	rol ARTH+4
	bne 3f
    1:	asl ARTH+4
    3:	rol ARTH+5
	rol ARTH+6
	rol ARTH+7
	rol ARTH+8
	rol ARTH+9
	rol ARTH+10
	rol ARTH+11
	dey
	bne 4b
	ldy UNSIGN
	beq 1f
	ldy SIGN
	beq 1f
	lda #0
	ldx #ARTH+4
	jsr Ngi4
    1:	rts


