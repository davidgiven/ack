.define Sri4, Sru4

! The subroutine Sri4 shifts a signed fourbyte integer to the
! right n times
! N is in register X.
! The subroutine Sru4 shifts an unsigned fourbyte integer to the
! right n times.

Sru4:
	txa
	tay
	bne 1f
	rts
    1:	jsr Pop
	stx ARTH
	sta ARTH+1
	jsr Pop
	stx ARTH+2
	jmp 2f
Sri4:
	txa
	tay
	bne 1f
	rts
    1:	jsr Pop
	stx ARTH
	sta ARTH+1
	jsr Pop
	stx ARTH+2
	tax
	bmi 1f
    2:	lsr a
	ror ARTH+2
	ror ARTH+1
	ror ARTH
    	dey
	bne 2b
	beq 4f
    1:	sec
	ror a
	ror ARTH+2
	ror ARTH+1
	ror ARTH
    3:	dey
	bne 1b
    4:	ldx ARTH+2
	jsr Push
	lda ARTH+1
	ldx ARTH
	jmp Push


