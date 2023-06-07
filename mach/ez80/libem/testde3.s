#
#include "asm.h"

.sect .text

.define .testde3
.testde3:
	push hl
	push bc
	push de
	pop hl
	and a
	ld bc, 0
	sbc hl, bc
	pop bc
	pop hl
	ret



