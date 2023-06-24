#
#include "asm.h"

.sect .text

! Shifts hl right by bc bits, signed.
.define .sri3
.sri3:
	push af
	push hl
	push bc

	ld hl, 5
	add hl, sp

1:
	sra (hl)
	dec hl
	rr (hl)
	dec hl
	rr (hl)
	inc hl
	inc hl
	djnz 1b

	pop bc
	pop hl
	pop af
	ret

