.define .mli2

!  16 bits signed integer multiply
!  the algorithm multiples A * B, where A = A0*2^8 + A1 and B = B0*2^8 + B1
!  product is thus A0*B0*2^16 + 2^8 * (A0 * B1 + B0 * A1) + A0 * B0
!  hence either A0 = 0 or B0 = 0 or overflow.
!  initial part of code determines which high byte is 0 (also for negative #s)
!  then the multiply is reduced to 8 x 16 bits, with the 8 bit number in the
!  a register, the 16 bit number in the hl register, and the product in de
!  Expects operands on stack
!  Yields result in de-registers

.mli2:	pop h
	shld .retadr	! get the return address out of the way
	lxi h,255
	pop d
	mov a,d		! check hi byte for 0
	cmp h		! h = 0
	jz 1f		! jump if de is a positive 8 bit number
	cmp l
	jz 5f		! jump if de is a negative 8 bit number
	xchg
	shld .tmp1	! we ran out of scratch registers
	pop h
	mov a,h
	cmp e
	jz 7f		! jump if second operand is 8 bit negative
	jmp 6f		! assume second operand is 8 bit positive

1:	mov a,e		! 8 bit positive number in a
	pop h		! 16 bit number in hl

! here is the main loop of the multiplication.  the a register is shifted
! right 1 bit to load the carry bit for testing.
! as soon as the a register goes to zero, the loop terminates.
! in most cases this requires fewer than 8 iterations.
2:	lxi d,0
	ora a
3:	rar		! load carry bit from a
	jnc 4f		! add hl to de if low bit was a 1
	xchg
	dad d
	xchg
4:	dad h
	ora a		! sets zero correct and resets carry bit
	jnz 3b		! if a has more bits, continue the loop
	lhld .retadr	! go get return address
	pchl

! the 8 bit operand is negative. negate both operands
5:	pop h
	mov a,l
	cma
	mov l,a
	mov a,h
	cma
	mov h,a
	inx h		! 16 bit negate is 1s complement + 1
	xra a
	sub e		! negate 8 bit operand
	jmp 2b

! second operand is small and positive
6:	mov a,l
	lhld .tmp1
	jmp 2b

! second operand is small and negative
7:	mov e,l
	lhld .tmp1
	mov a,l
	cma
	mov l,a
	mov a,h
	cma
	mov h,a
	inx h
	xra a
	sub e
	jmp 2b
