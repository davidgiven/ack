#
#include "asm.h"

.sect .text

! Shifts hlhl right by bc bits, signed.
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
	push hl
	sra (hl)
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
	pop hl
	djnz 1b

	pop bc
	pop hl
	exx
	pop hl
	exx
	pop af
	ret

