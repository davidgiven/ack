.sect .text

! Compare sets a, b.
!  Stack: ( b a size -- result )
!  Result is 0 if equal, nonzero if not equal.

.define .cms
.cms:
	lwz	r3, 0(sp)		! r3 = size of each set
	srwi	r7, r3, 2
	mtspr	ctr, r7			! ctr = size / 4
	addi	r4, sp, 4		! r4 = ptr to set a
	add	r5, r4, r3		! r5 = ptr to set b
	li	r6, 0			! r6 = index
1:
	lwzx	r7, r4, r6
	lwzx	r8, r5, r6
	cmpw	cr0, r7, r8		! compare words in sets
	addi	r6, r6, 4
	bne	cr0, 2f			! branch if not equal
	bdnz	1b			! loop ctr times
	li	r9, 0			! equal: return 0
	b	3f
2:
	li	r9, 1			! not equal: return 1
3:
	slwi	r7, r3, 1
	add	sp, sp, r7		! adjust stack pointer
	stw	r9, 0(sp)		! push result
	blr
