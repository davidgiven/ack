#
#include "asm.h"

.sect .text

! Any size logical or.
! Stack: ( a b size -- a|b )

.define .ior
.ior:
	push iy

	ld ix, 9
	add ix, sp		! first parameter

	ld bc, (ix-3)	! get size
	lea iy, ix
	add iy, bc		! second parameter

	! Actually perform the operation.

	ld hl, (ix-3)	! size again
	ld bc, -1
1:
	ld a, (ix)
	or (iy)
	ld (iy), a
	inc ix
	inc iy

	add hl, bc
	jr nz, 1b

	! We're finished. ix is pointing at the base of the second parameter.

	pop iy
	pop hl			! return address
	ld sp, ix		! retract stack
	jp (hl)
