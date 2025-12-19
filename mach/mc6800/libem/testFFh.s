.define TestFFh
.sect .zero
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! This subroutine tests if the value on top of the stack is 0< x <= 256.
! It is used if the istruction argument is on top of the stack.
! The value is saved in Y.


TestFFh:
	pshb
	psha
	subb #1
	sbca #0
	pula
	pulb
	bne 1f
2:	rts
1:	ldab #Eoddz
	ldaa #0
	jsr Trap
