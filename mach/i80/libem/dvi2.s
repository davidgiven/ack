.define .dvi2
.sect .text
.sect .rom
.sect .data
.sect .bss
.sect .text


! 16 bits signed and unsigned integer divide and remainder routine
! Bit 0 of a-reg is set iff quotient has to be delivered
! Bit 7 of a-reg is set iff the operands are signed, so:
! Expects in a-reg:	0 if called by rmu 2
!			1 if called by dvu 2
!			128 if called by rmi 2
!			129 if called by dvi 2
! Expects on stack:	divisor
!			dividend
! Yields in de-reg:	quotient or remainder

.dvi2:	pop h
	shld .retadr

	sta .areg
	pop b		! bc = divisor
	mov a,b		! trap if divisor = 0
	ora c
	cz eidivz
	pop d		! de = dividend

	mvi h,0
	lda .areg
	ral
	jnc 0f		! jump if unsigned

	mov a,d
	ral
	jnc 1f		! jump if dividend >= 0
	mvi h,129	! indicate dividend is negative
	xra a		! negate dividend
	sub e
	mov e,a
	mvi a,0
	sbb d
	mov d,a
			! de is positive now

1:	mov a,b
	ral
	jc 2f		! jump if divisor < 0
0:	inr h		! indicate negation
	xra a		! negate divisor
	sub c
	mov c,a
	mvi a,0
	sbb b
	mov b,a
			! bc is negative now 

2:	push h		! save h-reg
	lxi h,0		! initial value of remainder
	mvi a,16	! initialize loop counter

3:	push psw	! save loop counter
	dad h		! shift left: hl <- de <- 0
	xchg
	dad h
	xchg
	jnc 4f
	inx h

4:	push h		! save remainder
	dad b		! subtract divisor (add negative)
	jnc 5f
	xthl
	inx d

5:	pop h
	pop psw		! restore loop counter
	dcr a
	jnz 3b

	pop b		! b-reg becomes what once was h-reg
	lda .areg
	rar		! what has to be delivered: quotient or remainder?
	jnc 6f

! for dvi 2 and dvu 2 only:
	mov a,b
	rar
	jc 8f		! jump if divisor and dividend had same sign
	xra a		! negate quotient
	sub e
	mov e,a
	mvi a,0
	sbb d
	mov d,a
	jmp 8f

! for rmi 2 and rmu 2 only:
6:	mov a,b
	ral
	jnc 7f		! negate remainder if dividend was negative
	xra a
	sub l
	mov l,a
	mvi a,0
	sbb h
	mov h,a
7:	mov d,h		! return remainder
	mov e,l

8:	lhld .retadr
	pchl
