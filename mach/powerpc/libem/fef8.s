.sect .text; .sect .rom; .sect .data; .sect .bss

.sect .text

! Split a double-precision float into fraction and exponent, like
! frexp(3) in C.  On entry:
!  r3 = float, high word (bits 0..31)
!  r4 = float, low word (bits 32..63)
! Yields:
!  r3 = fraction, high word (bits 0..31)
!  r4 = fraction, low word (bits 32..63)
!  r5 = exponent

.define .fef8
.fef8:
	! IEEE double-precision format:
	!   sign  exponent  fraction
	!   0     1..11     12..63
	extrwi r6, r3, 11, 1		! r6 = IEEE exponent
	addi r5, r6, -1022		! r5 = true exponent
	cmpwi r6, 2047
	beqlr				! return if infinity or NaN
	cmpwi r6, 0
	bne 1f				! jump if normalized number

	! Got denormalized number or zero, probably zero.
	extrwi r6, r3, 22, 12
	addi r5, r0, 0			! r5 = true exponent = 0
	or. r6, r6, r4			! r6 = high|low fraction
	beqlr				! return if zero

	! Got denormalized number, not zero.
	stwu r4, -4(sp)
	stwu r3, -4(sp)
	lfd f0, 0(sp)
	lis r6, ha16[_2_64]
	lfd f1, lo16[_2_64](r6)
	fmul f0, f0, f1			! multiply it by 2**64
	stfd f0, 0(sp)
	lwz r3, 0(sp)
	lwz r4, 4(sp)
	extrwi r6, r3, 11, 1		! r6 = IEEE exponent
	addi sp, sp, 8
	addi r5, r6, -1022 - 64		! r5 = true exponent
1:
	! Put fraction in [0.5, 1) or (-1, -0.5] by setting its
	! exponent to true 0, IEEE 1022.
	rlwinm r3, r3, 0, 12, 0		! clear old exponent
	oris r3, r3, 1022 << 4		! set new exponent
	blr

.sect .rom
_2_64:
	! (double) 2**64
	.data4 0x43f00000
	.data4 0x00000000
