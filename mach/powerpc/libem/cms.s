.sect .text

! Compare sets a, b.
!  Stack: ( a b size -- result )
!  Result is 0 if equal, nonzero if not equal.

.define .cms
.cms:
	lwz	r3, 0(sp)		! r3 = size of each set
	srwi	r7, r3, 2
	mtspr	ctr, r7			! ctr = size / 4
	add	r4, sp, r3		! r4 = pointer before set a
	add	r7, r4, r3		! r7 = pointer to store result

	! Loop with r4 in a set a and sp in set b.
1:	lwzu	r5, 4(r4)
	lwzu	r6, 4(sp)
	cmpw	r5, r6			! compare words
	bne	2f			! branch if not equal
	bdnz	1b			! loop ctr times

	li	r3, 0			! equal: return 0
	b	3f
2:	li	r3, 1			! not equal: return 1
3:	mr	sp, r7
	stw	r3, 0(sp)		! push result
	blr
