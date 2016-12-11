#include "powerpc.h"

.sect .text

! Set union.
!  Stack: ( b a -- a+b )
!  With r3 = size of set

.define .ior
.ior:
	mr	r4, sp			! r4 = ptr to set a
	add	r5, sp, r3		! r5 = ptr to set b
	rlwinm	r6, r3, 30, 2, 31
	mtspr	ctr, r6			! ctr = r3 / 4
1:
	lwz	r7, 0(r4)
	lwz	r8, 0(r5)
	or	r8, r7, r8		! union of words
	stw	r8, 0(r5)
	addi	r4, r4, 4
	addi	r5, r5, 4
	bc	DNZ, 0, 1b		! loop ctr times
	add	sp, sp, r3
	bclr	ALWAYS, 0, 0
