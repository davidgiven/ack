.define .cms

! Any size compare
! Expects:	size in de-registers
!		operands on stack
! Yields in de-registers: 0 if operands are equal
!			  1 if operands are not equal

.cms:
	pop h
	shld .retadr

	mov l,e
	mov h,d
	mov a,l
	rar
	cc eoddz	!trap is size is odd
	dad sp		!now hl points to second operand
			!and sp points to the first.
1:	dcx sp
	pop psw		!get next byte in accumulator
	cmp m
	inx h
	dcx d
	jnz 2f		!jump if bytes are not equal
	mov a,d
	ora e
	jnz 1b
	jmp 3f
2:	dad d
	lxi d,1
3:	sphl

	lhld .retadr
	pchl
