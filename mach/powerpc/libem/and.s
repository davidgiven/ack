.sect .text

! Set intersection.
!  Stack: ( b a size -- a*b )

.define .and
.and:
	lwz	r3, 0(sp)		! r3 = size
	srwi	r7, r3, 2
	mtspr	ctr, r7			! ctr = size / 4
	addi	r4, sp, 4		! r4 = ptr to set a
	add	r5, r4, r3		! r5 = ptr to set b
	li	r6, 0			! r6 = index
1:
	lwzx	r7, r4, r6
	lwzx	r8, r5, r6
	and	r8, r7, r8		! intersection of words
	stwx	r8, r5, r6
	addi	r6, r6, 4
	bdnz	1b			! loop ctr times
	mr	sp, r5
	blr
