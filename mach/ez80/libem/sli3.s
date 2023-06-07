#
#include "asm.h"

.sect .text

.define .sli3
.sli3:
	push af
	push bc

	ld a, b
	or a
	jr z, 1f

	add hl, hl
	djnz .sli3

1:
	pop bc
	pop af
	ret

