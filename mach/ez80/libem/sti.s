#
#include "asm.h"

! Performs a variable-sized store.
! Stack is: ( data ptr size : - )

.sect .text
.define .sti
.sti:
	pop bc				! return address
	pop hl				! object size

	ld ix, 0
	add ix, sp			! ix is object pointer

1:
	pop de
	ld (ix), de
	lea ix, ix+3

	ld de, -3
	add hl, de
	jr nz, 1b

	push bc
	ret

