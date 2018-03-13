.sect .text; .sect .rom; .sect .data; .sect .bss

.sect .text

! Split a double-precision float into fraction and exponent, like
! frexp(3) in C, http://en.cppreference.com/w/c/numeric/math/frexp
!
! Stack: ( double -- fraction exponent )

.define .fef8
.fef8:
	lwz r3, 0(sp)			! r3 = high word (bits 0..31)
	lwz r4, 4(sp)			! r4 = low word (bits 32..63)

	! IEEE double = sign * 1.fraction * 2**(exponent - 1023)
	!   sign  exponent  fraction
	!   0     1..11     12..63
	!
	! IEEE exponent = 1022 in [0.5, 1) or (-1, -0.5].

	extrwi. r6, r3, 11, 1		! r6 = IEEE exponent
	beq 3f				! jump if zero or denormalized
	cmpwi r6, 2047
	addi r5, r6, -1022		! r5 = our exponent
	beq 2f				! jump if infinity or NaN
	! fall through if normalized

	! Put fraction in [0.5, 1) or (-1, -0.5].
1:	li r6, 1022
	insrwi r3, r6, 11, 1		! IEEE exponent = 1022
	! fall through

2:	stw r3, 0(sp)
	stw r4, 4(sp)			! push fraction
	stwu r5, -4(sp)			! push exponent
	blr

	! Got denormalized number or zero, probably zero.
	! If zero, then exponent must also be zero.
3:	extrwi r6, r3, 20, 12
	or. r6, r6, r4			! r6 = high|low fraction
	bne 4f				! jump if not zero
	li r5, 0			! exponent = 0
	b 2b

	! Got denormalized number = 0.fraction * 2**-1022
4:	lfd f0, 0(sp)
	lis r6, ha16[.fs_2_64]
	lfs f1, lo16[.fs_2_64](r6)
	fmul f0, f0, f1			! multiply it by 2**64
	stfd f0, 0(sp)
	lwz r3, 0(sp)
	lwz r4, 4(sp)
	extrwi r6, r3, 11, 1		! r6 = IEEE exponent
	addi r5, r6, -1022 - 64		! r5 = our exponent
	b 1b

.sect .rom
.fs_2_64:
	!float 1.84467440737095516e+19 sz 4
	.data1 0137,0200,00,00
