.sect .text

! Create empty set.
!  Stack: ( size -- set )

.define .zer
.zer:
	lwz	r3, 0(sp)		! r3 = size
	srwi	r5, r3, 2
	mtspr	ctr, r5			! ctr = word size - 4
	li	r4, 0			! r4 = 0
	addi	sp, sp, 4
	subf	sp, r3, sp		! sp = ptr to new set
	li	r6, 0			! r6 = index
1:
	stwx	r4, sp, r6		! store zero in set
	addi	r6, r6, 4
	bdnz	1b			! loop ctr times
	blr
