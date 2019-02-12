.define .csa
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text

! Case jump
! Expects on stack:	address of case descriptor
!			case index
! This is not a subroutine, but just a piece of code that computes
! the jump address and jumps to it.
! Traps if resulting address is zero.

.csa:	pop h		!hl = address of case descriptor
	pop d		!de = index
	mov c,m
	inx h
	mov b,m
	inx h
	push b		!save default pointer on stack
	mov a,e
	sub m
	inx h
	mov c,a
	mov a,d
	sbb m
	inx h
	mov b,a		!bc = index - lower bound
	jc 1f		!get default pointer
	mov a,m
	inx h
	sub c
	mov a,m
	inx h
	sbb b
	jc 1f		!upper-lower should be >= index-lower
	dad b
	dad b		!hl now points to the wanted pointer
	mov a,m
	inx h
	mov h,m
	mov l,a		!hl = pointer for index
	ora h
	jz 1f		!get default pointer if pointer = 0
	pop b		!remove default pointer
	pchl		!jump!!!!

1:	pop h		!get default pointer
	mov a,l
	ora h
	cz ecase	!trap
	pchl		!jump!!!!

