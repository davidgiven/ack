.define Rol4

! This subroutine rotates left a fourbyte integer n times.
! N is in register X.


Rol4:
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
    2:	asl ARTH
	rol ARTH+1
	rol ARTH+2
	rol ARTH+3	! rotate left
	bcc 1f
	inc ARTH	! put carry in rightmost bit
    1:	dey
	bne 2b
	ldx ARTH+2
	lda ARTH+3
	jsr Push
	ldx ARTH
	lda ARTH+1
	jmp Push


