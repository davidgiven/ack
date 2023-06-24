#
#include "asm.h"

.sect .text

! Any size logical exchange.
! Stack: ( a b size -- b a )

.define .exg
.exg:
	pop de			! return address
	pop bc			! size
	push hl			! reconstruct stack frame
	push iy

	ld ix, 3
	add ix, sp		! first parameter

	lea iy, ix
	add iy, de		! second parameter

	! Actually perform the operation.

	ex de, hl		! size into hl
	ld bc, -1
1:
	ld d, (ix)
	ld e, (iy)
	ld (iy), d
	ld (ix), e
	inc ix
	inc iy

	add hl, bc
	jr nz, 1b

	! Finished!

	pop iy
	ret

