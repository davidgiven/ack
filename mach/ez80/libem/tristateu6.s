#
#include "asm.h"

.sect .text

! Immediately after a subtraction, sets HL to a -1/0/1 tristate value
! reflecting the result of the comparison. Unsigned.

.define .tristateu6
.tristateu6:
	ld hl, 0
	ret z			! equal
	jr c, negative
	inc hl
	ret
negative:
	dec hl
	ret

