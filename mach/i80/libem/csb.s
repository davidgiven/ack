.define .csb
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! Table lookup jump
! Expects on stack:	address of case descriptor
!			case index
! This is not a subroutine, but just a piece of code that computes
! the jump address and jumps to it.
! Traps if resulting address is zero.

.csb:	pop h		!hl = pointer to descriptor
	pop d		!de = case index
	mov c,m		!bc = default pointer
	inx h
	mov b,m
	inx h
	push b		!save default on stack
	mov c,m		!bc = number of entries
	inx h
	mov b,m
	inx h
!loop: try to find the case index in the descriptor
1:	mov a,b
	ora c
	jz 4f		!done, index not found
	mov a,m		!do we have the right index?
	inx h
	cmp e
	jnz 2f		!no
	mov a,m
	inx h
	cmp d
	jnz 3f		!no
	mov a,m
	inx h
	mov h,m
	mov l,a
	pop psw		!remove default pointer
	jmp 5f

2:	inx h		!skip high byte of index
3:	inx h		!skip jump address
	inx h
	dcx b
	jmp 1b

4:	pop h		!take default exit
5:	
	mov a,l		!jump address is zero?
	ora h
	cz ecase	!trap
	pchl		!jump!!!!

