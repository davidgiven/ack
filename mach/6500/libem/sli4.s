.define Sli4

! This subroutine shift a signed or unsigned fourbyte integer
! n times left. N is in register X.


Sli4:
	cpx #0
	beq 9f		! zero shift, return input
	lda SP+2	! the shifting is done on the stack
	sta ADDR	! address of integer (lowbyte)
	lda SP+1
	sta ADDR+1	! address of integer (highbyte)
    2:	ldy #0
	clc
	lda (ADDR),y
	rol a
	sta (ADDR),y
	iny
	lda (ADDR),y
	rol a
	sta (ADDR),y
	iny
	lda (ADDR),y
	rol a
	sta (ADDR),y
	iny
	lda (ADDR),y
	rol a
	sta (ADDR),y	! shift left
	dex
	bne 2b
    9:	rts


