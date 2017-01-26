.sect .text

! Set union.
!  Stack: ( b a size -- a+b )

.define .ior
.ior:
	lwz r3, 0 (sp)
	addi sp, sp, 4

	mr	r4, sp			! r4 = ptr to set a
	add	r5, sp, r3		! r5 = ptr to set b
	srwi	r6, r3, 2
	mtspr	ctr, r6			! ctr = r3 / 4
1:
	lwz	r7, 0(r4)
	lwz	r8, 0(r5)
	or	r8, r7, r8		! union of words
	stw	r8, 0(r5)
	addi	r4, r4, 4
	addi	r5, r5, 4
	bdnz	1b			! loop ctr times
	add	sp, sp, r3
	blr
