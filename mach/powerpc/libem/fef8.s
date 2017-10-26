.sect .text; .sect .rom; .sect .data; .sect .bss

.sect .text

! Split a double-precision float into fraction and exponent, like
! frexp(3) in C.
!
! Stack: ( double -- fraction exponent )

.define .fef8
.fef8:
	lwz r3, 0(sp)			! r3 = high word (bits 0..31)
	lwz r4, 4(sp)			! r4 = low word (bits 32..63)

	! IEEE double-precision format:
	!   sign  exponent  fraction
	!   0     1..11     12..63
	!
	! To get fraction in [0.5, 1) or (-1, -0.5], we subtract 1022
	! from the IEEE exponent.

	extrwi. r6, r3, 11, 1		! r6 = IEEE exponent
	addi r5, r6, -1022		! r5 = our exponent
	beq 2f				! jump if zero or denormalized
	cmpwi r6, 2047
	beq 1f				! jump if infinity or NaN
	! fall through if normalized

	! Put fraction in [0.5, 1) or (-1, -0.5] by setting its
	! IEEE exponent to 1022.
	rlwinm r3, r3, 0, 12, 0		! clear old exponent
	oris r3, r3, 1022 << 4		! set new exponent
	! fall through

1:	stw r3, 0(sp)
	stw r4, 4(sp)			! push fraction
	stwu r5, -4(sp)			! push exponent
	blr

2:	! Got denormalized number or zero, probably zero.
	extrwi r6, r3, 22, 12
	or. r6, r6, r4			! r6 = high|low fraction
	bne 3f				! jump if not zero
	li r5, 0			! exponent = 0
	b 1b

3:	! Got denormalized number, not zero.
	lfd f0, 0(sp)
	lis r6, ha16[_2_64]
	lfd f1, lo16[_2_64](r6)
	fmul f0, f0, f1			! multiply it by 2**64
	stfd f0, 0(sp)
	lwz r3, 0(sp)
	lwz r4, 4(sp)
	extrwi r6, r3, 11, 1		! r6 = IEEE exponent
	addi r5, r6, -1022 - 64		! r5 = our exponent
	b 1b

.sect .rom
_2_64:
	! (double) 2**64
	.data4 0x43f00000
	.data4 0x00000000
