.define Inn

! This subroutine checks if a certain bit is set in a set
! of n bytes on top of the stack.


Inn:
	stx ARTH	! save bit number (lowbyte)
	sta ARTH+1	! save bit number (highbyte)
	and #80h
	beq 1f
	lda #0		! bit number is negative
	sta ARTH+2	! make it zero
	beq 3f
    1:	txa
	and #07h	! get bit number mod 8
	tax
	lda #1
	cpx #0		! bit number = 0
	beq 2f		! no shifting to right place
    1:	asl a		! shift left until bit is in place
	dex
	bne 1b
    2:	sta ARTH+2	! bit is one in place
	ldx #3
    1:	lsr ARTH+1	! shift left 3 times bit number (highbyte)
	ror ARTH	! shift left 3 times bit number (lowbyte)
	dex		! this is bit number div 8
	bne 1b		! which is byte number
    3:	lda SP+1
	ldx SP+2
	stx ADDR	! address of the set (lowbyte)
	sta ADDR+1	! address of the set (highbyte)
	iny
	tya
	bne 2f
	inc SP+1
    2:	clc		! remove the set
	adc SP+2
	sta SP+2	! new stackpointer (lowbyte)
	lda #0
	adc SP+1
	sta SP+1	! new stackpointer (highbyte)
	ldy ARTH
	lda (ADDR),y	! load the byte in A
	bit ARTH+2	! test bit
	bne 1f
    3:	lda #0		! bit is zero
	tax
	rts
    1:	lda #0		! bit is one
	ldx #1
	rts


