.define Zer
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! This subroutine puts n (n <=256) zero bytes on top of 
! the stack.
! The number of bytes minus one is in Y.


Zer:
	tya
	lsr a		! number of bytes div 2
	tay
	iny
	lda #0
	tax
    2:	jsr Push	! push two bytes
	dey
	bne 2b
	rts


