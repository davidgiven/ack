#
#include "asm.h"

.sect .text

! NOTs hl.
.define .not3
.not3:
	push hl
	push af

	ld hl, 3
	add hl, sp

	ld a, (hl)
	cpl
	ld (hl), a
	inc hl

	ld a, (hl)
	cpl
	ld (hl), a
	inc hl

	ld a, (hl)
	cpl
	ld (hl), a

	pop af ! don't overwrite hl
	pop af
	ret


