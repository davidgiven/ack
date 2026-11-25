.define Lar
.sect .zero
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! Load an array element onto the EM stack.
! AccAB holds the descriptor. The index and array base are on the stack.

Lar:
	tsx
	ldx 0,x
	stx <TMP2	! save the return address; Aar preserves TMP2
	ins
	ins
	stab <ADDR+1
	staa <ADDR
	ldx <ADDR
	ldx 4,x
	stx <NBYTES
	jsr Aar
	stab <ADDR+1
	staa <ADDR
	ldx <NBYTES
	cpx #1
	beq 2f

	addb <NBYTES+1
	adca <NBYTES
	stab <ADDR+3
	staa <ADDR+2
	ldx <ADDR+2
1:
	dex
	ldab 0,x
	pshb
	cpx <ADDR
	bne 1b
3:
	ldx <TMP2
	jmp 0,x		! return with the element on the stack

2:
	ldx <ADDR
	ldab 0,x
	clra		! zero-extend to one EM word
	pshb
	psha
	bra 3b
