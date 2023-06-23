#
#include "asm.h"

.sect .text

.define .sli3
.sli3:
	push af
	push bc

	ld a, c
	or a
	jr z, 1f

2:
	add hl, hl
	djnz 2b

1:
	pop bc
	pop af
	ret

