.sect .text

! Does a block move of words between non-overlapping buffers.
!  Stack: ( src dst len -- )

.define .bls4
.bls4:
	lwz	r3, 0(sp)	! len
	lwz	r4, 4(sp)	! dst
	lwz	r5, 8(sp)	! src
	addi	sp, sp, 12
	srwi	r3, r3, 2
	mtspr	ctr, r3
	addi	r5, r5, -4
	addi	r4, r4, -4
1:	lwzu	r3, 4(r5)
	stwu	r3, 4(r4)
	bdnz	1b
	blr
