.define Ngi4
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! This subroutine takes a fourbyte interger and negates it.
! For more details see ngi2.s


Ngi4:
	sta ADDR+1
	stx ADDR
	ldy #3
    1:	lda (ADDR),y
	eor #0FFh	! one's complement lowbyte+y
	sta (ADDR),y
	dey
	bpl 1b
	ldx #0FDh
	iny
	clc
    	lda (ADDR),y
	adc #1
	sta (ADDR),y	! lowbyte+y
    1:	iny
	lda (ADDR),y
	adc #0
	sta (ADDR),y	! (lowbyte+y)+0
	inx
	bne 1b
	rts


