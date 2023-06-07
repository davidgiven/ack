#
#include "asm.h"

.sect .text

.define .testhl3
.testhl3:
	push hl
	push de
	and a
	ld de, 0
	sbc hl, de
	pop de
	pop hl
	ret

