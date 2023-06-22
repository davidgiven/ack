#
#include "asm.h"

.sect .text

! NOTs hl.
.define .not6
.not6:
	push de

	ex de, hl
	ld hl, 0		! negate
	exx
	ex de, hl
	ld hl, 0
	exx

	and a
	sbc hl, de
	exx
	sbc hl, de
	exx

	pop de
	ret


