#
#include "asm.h"

.sect .text

! Immediately after a subtraction, sets HL to a -1/0/1 tristate value
! reflecting the result of the comparison. Signed.

.define .tristates6
.tristates6:
	ld hl, 0
	ret z			! equal
	jp po, negative
	inc hl
	ret
negative:
	dec hl
	ret


