#
#include "asm.h"

.sect .text

! Shifts hl right by bc bits.
.define .sru6
.sru6:
	push af
	exx
	push hl
	exx
	push hl
	push bc

	ld hl, 8
	add hl, sp

1:
	srl (hl)
	dec hl
	rr (hl)
	dec hl
	rr (hl)
	dec hl
	rr (hl)
	dec hl
	rr (hl)
	dec hl
	rr (hl)
	inc hl
	inc hl
	inc hl
	inc hl
	inc hl
	djnz 1b

	pop bc
	pop hl
	exx
	pop hl
	exx
	pop af
	ret

