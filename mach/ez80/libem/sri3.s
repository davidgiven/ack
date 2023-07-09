#
#include "asm.h"

.sect .text

! Shifts hl right by bc bits, signed.
.define .sri3
.sri3:
	push af
	push hl
	push bc

	ld a, c
	or a
	jr z, exit
	ld b, a

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

exit:
	pop bc
	pop hl
	pop af
	ret

