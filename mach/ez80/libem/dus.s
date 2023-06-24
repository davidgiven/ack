#
#include "asm.h"

.sect .text

! Any size duplicate.
! Stack: ( a size -- a a )

.define .dus
.dus:
	pop bc			! return address
	pop de			! size

	ld hl, 0
	and a
	sbc hl, de		! invert size
	add hl, sp
	ld sp, hl		! new stack pointer

	push bc
	push iy
	ld iy, 3
	add iy, sp		! pointer to new structure

	lea ix, iy
	add ix, de		! pointer to old structure

	ex de, hl		! size into hl

	! Actually perform the operation.

	ld bc, -1
1:
	ld a, (ix)
	ld (iy), a
	inc ix
	inc iy

	add hl, bc
	jr nz, 1b

	! Finished!

	pop iy
	ret

