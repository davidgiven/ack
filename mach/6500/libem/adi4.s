.define Adi4
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! This subroutine adds two fourbyte integers, which are on the stack.
! The addresses are initiated by the subroutine Addsub.
! Also the loopvariable (register X) is initiated by that routine.
! The result is pushed back onto the stack


Adi4:
	jsr Addsub		! initiate addresses
	clc
    1:	lda (ADDR+2),y		! get byte first operand
	adc (ADDR),y		! add to byte second operand
	sta (ADDR+2),y		! push on real stack
	iny
	inx
	bne 1b			! do it 4 times
	rts


