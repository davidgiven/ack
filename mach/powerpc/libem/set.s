.sect .text

! Create singleton set.
!  Stack: ( bitnumber size -- set )

.define .set
.set:
	lwz     r3, 0 (sp)
	lwz     r4, 4 (sp)
	addi    sp, sp, 8

	srwi	r7, r3, 2
	neg	r5, r3
	add	sp, sp, r5		! allocate set
	mr	r6, sp			! r6 = ptr to set
	mtspr	ctr, r7			! ctr = r3 / 4
1:
	clrrwi.	r7, r4, 5		! r7 = r4 & ~31
	beq	2f			! branch if r4 in 0..31
	li	r5, 0			! no bit, word is zero
	b	3f
2:
	li	r5, 1
	slw	r5, r5, r4		! yes bit, set bit in word
3:
	stw	r5, 0(r6)		! store word in set
	addi	r4, r4, -32
	addi	r6, r6, 4
	bdnz	1b			! loop ctr times
	blr
