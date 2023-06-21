#
#include "asm.h"

.sect .text

! Pops size, dest, src from stack.
.define .bls
.bls:
	push iy
	ld iy, 0
	add iy, sp

	ld bc, (iy+6)
	ld de, (iy+9)
	ld hl, (iy+12)
	ldir

	pop iy
	ret

