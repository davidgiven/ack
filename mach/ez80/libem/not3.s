#
#include "asm.h"

.sect .text

! NOTs hl.
.define .not3
.not3:
	push de

	ex de, hl
	ld hl, 0		! negate
	and a
	sbc hl, de

	dec hl			! not = negate . decrement

	pop de
	ret

