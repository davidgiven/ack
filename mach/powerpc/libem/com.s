.sect .text

! Set complement.
!  Stack: ( a size -- ~a )

.define .com
.com:
	lwz r3, 0 (sp)       ! size
	addi sp, sp, 4

	mr	r4, sp			! r4 = pointer to set a
	srwi	r5, r3, 2
	mtspr	ctr, r5			! ctr = r3 / 4
1:
	lwz	r6, 0(r4)
	nor	r6, r6, r6		! complement of word
	stw	r6, 0(r4)
	addi	r4, r4, 4
	bdnz	1b			! loop ctr times
	blr
