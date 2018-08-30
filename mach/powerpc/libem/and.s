.sect .text

! Set intersection.
!  Stack: ( a b size -- a*b )

.define .and
.and:
	lwz	r3, 0(sp)		! r3 = size
	srwi	r7, r3, 2
	mtspr	ctr, r7			! ctr = size / 4
	add	r4, sp, r3		! r4 = pointer before set a

	! Loop with r4 in set a and sp in set b.
1:	lwzu	r5, 4(r4)
	lwzu	r6, 4(sp)
	and	r7, r5, r6		! intersection of words
	stw	r7, 0(r4)
	bdnz	1b			! loop ctr times
	addi	sp, sp, 4		! drop last word of set b
	blr
