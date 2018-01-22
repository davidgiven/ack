.sect .text

! Split a single-precision float into fraction and exponent, like
! frexpf(3) in C, http://en.cppreference.com/w/c/numeric/math/frexp
!
! Stack: ( single -- fraction exponent )

.define .fef4
.fef4:
	lwz r3, 0(sp)			! r3 = word of float bits

	! IEEE single = sign * 1.fraction * 2**(exponent - 127)
	!   sign  exponent  fraction
	!   0     1..8      9..31
	!
	! IEEE exponent = 126 in [0.5, 1) or (-1, -0.5].

	extrwi. r6, r3, 8, 1		! r6 = IEEE exponent
	beq 3f				! jump if zero or denormalized
	cmpwi r6, 255
	addi r5, r6, -126		! r5 = our exponent
	beq 2f				! jump if infinity or NaN
	! fall through if normalized

	! Put fraction in [0.5, 1) or (-1, -0.5].
1:	li r6, 126
	insrwi r3, r6, 8, 1		! IEEE exponent = 126
	! fall through

2:	stw r3, 0(sp)			! push fraction
	stwu r5, -4(sp)			! push exponent
	blr

	! Got denormalized number or zero, probably zero.
	! If zero, then exponent must also be zero.
3:	extrwi. r6, r3, 23, 9		! r6 = fraction
	bne 4f				! jump if not zero
	li r5, 0			! exponent = 0
	b 2b

	! Got denormalized number = 0.fraction * 2**-126
4:	cntlzw r5, r6
	addi r5, r5, -8
	slw r6, r6, r5			! shift left to make 1.fraction
	insrwi r3, r6, 23, 9		! set new fraction
	li r6, -126 + 1
	subf r5, r5, r6			! r5 = our exponent
	b 1b
