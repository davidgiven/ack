.define TestFFh
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! This subroutine tests if the value on top of the stack is <= 256.
! It is used if the istruction argument is on top of the stack.
! The value is saved in Y.


TestFFh:
	cmp #2
	bpl 1f		! value > 256
	cmp #0
	beq 2f
	cpx #0
	bne 1f		! value is zero
    2:	dex
	txa
	tay
	rts
    1:	ldx #Eoddz
	lda #0
	jsr Trap


