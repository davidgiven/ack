.sect .text

! Multiplies two single-precision floats, then splits the product into
! fraction and integer, both as floats, like modff(3) in C,
! http://en.cppreference.com/w/c/numeric/math/modf
!
! Stack: ( a b -- fraction integer )

.define .fif4
.fif4:
	lfs f1, 4(sp)
	lfs f2, 0(sp)
	fmuls f1, f1, f2		! f1 = a * b
	stfs f1, 0(sp)
	lwz r3, 0(sp)			! r3 = word of float bits

	! IEEE single = sign * 1.fraction * 2**(exponent - 127)
	!   sign  exponent  fraction
	!   0     1..8      9..31
	!
	! Subtract 127 from the IEEE exponent.  If the result is from
	! 0 to 23, then the IEEE fraction has that many integer bits.

	extrwi r5, r3, 8, 1		! r5 = IEEE exponent
	addic. r5, r5, -127		! r5 = nr of integer bits
	blt 3f				! branch if no integer
	cmpwi r5, 24
	bge 4f				! branch if no fraction
	! fall through if integer with fraction

	! f1 has r5 = 0 to 23 integer bits in the IEEE fraction.
	! There are 23 - r5 fraction bits.
	li r6, 23
	subf r6, r5, r6
	srw r3, r3, r6
	slw r3, r3, r6			! clear fraction in word
	! fall through

1:	stw r3, 0(sp)
	lfs f2, 0(sp)			! integer = high word, low word
	fsubs f1, f1, f2		! fraction = value - integer
2:	stfs f1, 4(sp)			! push fraction
	stfs f2, 0(sp)			! push integer
	blr

	! f1 is a fraction without integer (or zero).
	! Then integer is zero with same sign.
3:	extlwi r3, r3, 1, 0		! extract sign bit
	stfs f1, 4(sp)			! push fraction
	stw r3, 0(sp)			! push integer = zero with sign
	blr

	! f1 is an integer without fraction (or infinity or NaN).
	! Unless NaN, then fraction is zero with same sign.
4:	fcmpu cr0, f1, f1
	bun cr0, 5f
	extlwi r3, r3, 1, 0		! extract sign bit
	stw r3, 4(sp)			! push fraction = zero with sign
	stfs f1, 0(sp)			! push integer
	blr

	! f1 is NaN, so both fraction and integer are NaN.
5:	fmr f2, f1
	b 2b
