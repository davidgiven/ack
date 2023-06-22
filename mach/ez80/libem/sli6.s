#
#include "asm.h"

.sect .text

.define .sli6
.sli6:
	push af
	push bc

	ld a, b
	or a
	jr z, 1f

2:
	add hl, hl
	exx
	adc hl, hl
	exx
	djnz 2b

1:
	pop bc
	pop af
	ret

