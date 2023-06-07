#
#include "asm.h"

.sect .text

.define .testbc3
.testbc3:
	push hl
	push de
	push bc
	pop hl
	and a
	ld de, 0
	sbc hl, de
	pop de
	pop hl
	ret


