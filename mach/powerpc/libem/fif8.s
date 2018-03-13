.sect .text

! Multiplies two double-precision floats, then splits the product into
! fraction and integer, both as floats, like modf(3) in C,
! http://en.cppreference.com/w/c/numeric/math/modf
!
! Stack: ( a b -- fraction integer )

.define .fif8
.fif8:
	lfd f1, 8(sp)
	lfd f2, 0(sp)
	fmul f1, f1, f2			! f1 = a * b
	stfd f1, 0(sp)
	lwz r3, 0(sp)			! r3 = high word
	lwz r4, 4(sp)			! r4 = low word

	! IEEE double = sign * 1.fraction * 2**(exponent - 1023)
	!   sign  exponent  fraction
	!   0     1..11     12..63
	!
	! Subtract 1023 from the IEEE exponent.  If the result is from
	! 0 to 51, then the IEEE fraction has that many integer bits.

	extrwi r5, r3, 11, 1		! r5 = IEEE exponent
	addic. r5, r5, -1023		! r5 = nr of integer bits
	blt 3f				! branch if no integer
	cmpwi r5, 52
	bge 4f				! branch if no fraction
	cmpwi r5, 21
	bge 6f				! branch if large integer
	! fall through if small integer

	! f1 has r5 = 0 to 20 integer bits in the IEEE fraction.
	! High word has 20 - r5 fraction bits.
	li r6, 20
	subf r6, r5, r6
	srw r3, r3, r6
	li r4, 0			! clear low word
	slw r3, r3, r6			! clear fraction in high word
	! fall through

1:	stw r3, 0(sp)
	stw r4, 4(sp)
	lfd f2, 0(sp)			! integer = high word, low word
	fsub f1, f1, f2			! fraction = value - integer
2:	stfd f1, 8(sp)			! push fraction
	stfd f2, 0(sp)			! push integer
	blr

	! f1 is a fraction without integer (or zero).
	! Then integer is zero with same sign.
3:	extlwi r3, r3, 1, 0		! extract sign bit
	li r4, 0
	stfd f1, 8(sp)			! push fraction
	stw r4, 4(sp)
	stw r3, 0(sp)			! push integer = zero with sign
	blr

	! f1 is an integer without fraction (or infinity or NaN).
	! Unless NaN, then fraction is zero with same sign.
4:	fcmpu cr0, f1, f1		! integer = f1
	bun cr0, 5f
	extlwi r3, r3, 1, 0		! extract sign bit
	li r4, 0
	stw r4, 12(sp)
	stw r3, 8(sp)			! push fraction = zero with sign
	stfd f1, 0(sp)			! push integer
	blr

	! f1 is NaN, so both fraction and integer are NaN.
5:	fmr f2, f1
	b 2b

	! f1 has r5 = 21 to 51 to integer bits.
	! Low word has 52 - r5 fraction bits.
6:	li r6, 52
	subf r6, r5, r6
	srw r4, r4, r6
	slw r4, r4, r6			! clear fraction in low word
	b 1b
