#include "powerpc.h"

.sect .text

! Compare sets a, b.
!  Stack: ( b a -- )
!  With r3 = size of each set
!  Yields r3 = 0 if equal, nonzero if not equal

.define .cms
.cms:
	mr	r4, sp			! r4 = ptr to set a
	add	r5, sp, r3		! r5 = ptr to set b
	mr	r6, r3			! r6 = size
	rlwinm	r3, r3, 30, 2, 31
	mtspr	ctr, r3			! ctr = size / 4
1:
	lwz	r7, 0(r4)
	lwz	r8, 0(r5)
	cmp	cr0, 0, r7, r8		! compare words in sets
	addi	r4, r4, 4
	addi	r5, r5, 4
	bc	IFFALSE, EQ, 2f		! branch if not equal
	bc	DNZ, 0, 1b		! loop ctr times
	addi	r3, r0, 0		! equal: return 0
	b	3f
2:
	addi	r3, r0, 1		! not equal: return 1
3:
	rlwinm	r6, r6, 1, 0, 30	! r6 = size * 2
	add	sp, sp, r6		! remove sets from stack
	bclr	ALWAYS, 0, 0
