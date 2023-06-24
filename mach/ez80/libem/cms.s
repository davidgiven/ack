#
#include "asm.h"

.sect .text

! Any size comparison.
! Stack: ( a b size -- a!=b )

.define .cms
.cms:
	push iy

	ld ix, 9
	add ix, sp		! first parameter

	ld bc, (ix-3)	! get size
	lea iy, ix
	add iy, bc		! second parameter

	! Actually perform the operation.

	ld hl, (ix-3)	! size again
	ld bc, -1
	ld de, 1		! result --- different
1:
	ld a, (ix)
	cp (iy)
	jr nz, different
	inc ix
	inc iy

	add hl, bc
	jr nz, 1b

	dec de			! the same
different:

	! We're finished. iy is pointing at the end of the second parameter.

	pop ix
	pop hl			! return address
	ld sp, iy		! retract stack
	lea iy, ix
	push de
	jp (hl)
