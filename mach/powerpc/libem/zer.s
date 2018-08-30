.sect .text

! Create empty set.
!  Stack: ( size -- set )

.define .zer
.zer:
	lwz	r3, 0(sp)		! r3 = size
	srwi	r7, r3, 2
	mtspr	ctr, r7			! ctr = size / 4
	addi	sp, sp, 4		! drop size from stack
	li	r4, 0

1:	stwu	r4, -4(sp)		! push zero
	bdnz	1b			! loop ctr times
	blr
