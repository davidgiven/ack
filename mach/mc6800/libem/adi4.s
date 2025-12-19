.define Adi4
.sect .zero
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! This subroutine adds two fourbyte integers, which are on the stack.
! The result is pushed back onto the stack

! +0    rerurn address
! +2,+5 4byte integer
! +6,+9 4byte integer (and result)

Adi4:
	tsx
	ldab 9,x
	ldaa 8,x
	addb 5,x
	adca 4,x
	stab 9,x
	staa 8,x
	ldab 7,x
	ldaa 6,x
	adcb 3,x
	adca 2,x
	stab 7,x
	staa 6,x
	ldx 0,x
	ins
	ins
	ins
	ins
	ins
	ins
	jmp 0,x
