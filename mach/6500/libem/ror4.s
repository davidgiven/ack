.define Ror4

! This subroutine rotates right a fourbyte word.
! The number of rotates is in X.


Ror4:
	txa
	bne 1f		! a zero rotate skip
	rts
    1:	tay
	jsr Pop
	stx ARTH
	sta ARTH+1
	jsr Pop
	stx ARTH+2
	sta ARTH+3
    2:	lsr ARTH+3	! rotate word
	ror ARTH+2
	ror ARTH+1
	ror ARTH
	bcc 1f		! no carry
	lda #80h	! put carry in leftmost bit
	ora ARTH+3
	sta ARTH+3
    1:	dey
	bne 2b
	lda ARTH+3
	ldx ARTH+2
	jsr Push
	lda ARTH+1
	ldx ARTH
	jmp Push	! push result onto the stack


