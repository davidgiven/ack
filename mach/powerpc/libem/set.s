#include "powerpc.h"

.sect .text

! Create singleton set.
!  Stack: ( -- set )
!  With r3 = size of set, r4 = bit number

.define .set
.set:
	rlwinm	r7, r3, 30, 2, 31
	neg	r5, r3
	add	sp, sp, r5		! allocate set
	mr	r6, sp			! r6 = ptr to set
	mtspr	ctr, r7			! ctr = r3 / 4
1:
	rlwinm.	r7, r4, 0, 0, 26	! r7 = r4 & ~31
	bc	IFTRUE, EQ, 2f		! branch if r4 in 0..31
	addi	r5, r0, 0		! no bit, word is zero
	b	3f
2:
	addi	r5, r0, 1
	slw	r5, r5, r4		! yes bit, set bit in word
3:
	stw	r5, 0(r6)		! store word in set
	addi	r4, r4, -32
	addi	r6, r6, 4
	bc	DNZ, 0, 1b		! loop ctr times
	bclr	ALWAYS, 0, 0
