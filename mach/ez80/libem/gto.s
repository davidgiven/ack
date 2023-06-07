#
#include "asm.h"

.sect .text

.define .gto
.gto:
	push hl
	pop ix

	ld hl, (ix+3)	/* new sp */
	ld sp, hl

	ld hl, (ix+0)	/* new pc */

	ld iy, (ix+6)	/* new fp */

	jp (hl)

