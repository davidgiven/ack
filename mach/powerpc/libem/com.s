.sect .text

! Set complement.
!  Stack: ( a size -- ~a )

.define .com
.com:
	lwz	r3, 0(sp)		! r3 = size
	srwi	r7, r3, 2
	mtspr	ctr, r7			! ctr = size / 4
	mr	r4, sp			! r4 = pointer before set a

	! Loop with r4 in set a.
1:	lwzu	r5, 4(r4)
	nor	r7, r5, r5		! complement of word
	stw	r7, 0(r4)
	bdnz	1b			! loop ctr times
	addi	sp, sp, 4		! drop size from stack
	blr
