.sect .text

! Compare sets a, b.
!  Stack: ( b a -- )
!  With r3 = size of each set
!  Yields r3 = 0 if equal, nonzero if not equal

.define .cms
.cms:
	srwi	r7, r3, 2
	mtspr	ctr, r7			! ctr = size / 4
	mr	r4, sp			! r4 = ptr to set a
	add	r5, sp, r3		! r5 = ptr to set b
	li	r6, 0			! r6 = index
	add	r9, r5, r3		! r9 = future sp
1:
	lwzx	r7, r4, r6
	lwzx	r8, r5, r6
	cmpw	cr0, r7, r8		! compare words in sets
	addi	r6, r6, 4
	bne	cr0, 2f			! branch if not equal
	bdnz	1b			! loop ctr times
	li	r3, 0			! equal: return 0
	b	3f
2:
	li	r3, 1			! not equal: return 1
3:
	mr	sp, r9			! remove sets from stack
	blr
