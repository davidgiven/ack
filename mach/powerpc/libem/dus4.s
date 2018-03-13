.sect .text

! Duplicates some words on top of stack.
!  Stack: ( a size -- a a )

.define .dus4
.dus4:
	lwz	r3, 0(sp)
	addi	sp, sp, 4
	srwi	r4, r3, 2
	mtspr	ctr, r4
	add	r5, sp, r3
1:	lwzu	r4, -4(r5)
	stwu	r4, -4(sp)
	bdnz	1b
	blr
