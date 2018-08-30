.sect .text

! Exchange top two values on stack.
!   Stack: ( a b size -- b a )

.define .exg
.exg:
	lwz	r3, 0(sp)		! r3 = size
	srwi	r7, r3, 2
	mtspr	ctr, r7			! ctr = size / 4
	mr	r4, sp			! r4 = pointer before value b
	add	r5, r4, r3		! r5 = pointer before value a

	! Loop to swap each pair of words.
1:	lwzu	r6, 4(r4)
	lwzu	r7, 4(r5)
	stw	r6, 0(r5)
	stw	r7, 0(r4)
	bdnz	1b			! loop ctr times

	addi	sp, sp, 4		! drop size from stack
	blr
