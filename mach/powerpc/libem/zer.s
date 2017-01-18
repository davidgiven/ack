#include "powerpc.h"

.sect .text

! Create empty set.
!  Stack: ( size -- set )

.define .zer
.zer:
	lwz     r3, 0(sp)
	addi    sp, sp, 4

	rlwinm	r7, r3, 30, 2, 31
	addi	r4, r0, 0		! r4 = zero
	neg	r5, r3
	add	sp, sp, r5		! allocate set
	mr	r6, sp			! r6 = ptr to set
	mtspr	ctr, r7			! ctr = r3 / 4
1:
	stw	r4, 0(r6)		! store zero in set
	addi	r6, r6, 4
	bc	DNZ, 0, 1b		! loop ctr times
	bclr	ALWAYS, 0, 0
