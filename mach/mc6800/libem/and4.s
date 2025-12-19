.define And4
.sect .zero
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! This subroutine performs logical and on  two fourbyte integers,
! which are on the stack.
! The result is pushed back onto the stack

! +0    rerurn address
! +2,+5 4byte integer
! +6,+9 4byte integer (and result)

And4:
	tsx
	ldab 9,x
	ldaa 8,x
	andb 5,x
	anda 4,x
	stab 9,x
	staa 8,x
	ldab 7,x
	ldaa 6,x
	andb 3,x
	anda 2,x
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
