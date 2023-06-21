#
#include "asm.h"

/* Divides two forty-eight-bit signed numbers: DEDE = DEDE / BCBC
 * The quotient is returned in DEDE, the remainder in HLHL.
 */

.sect .text
.define .rmsdvs6
.rmsdvs6:
	push af
	push iy
	ld iy, 0
	add iy, sp

	exx
	push de				/* lhs high */
	push bc				/* rhs high */
	exx

	ld a, (iy-1)
	jp p, 1f
	ld hl, 0			/* invert de to make it positive */
	push hl
	exx
	pop hl
	exx
	and a
	sbc hl, de
	ex de, hl
	exx
	sbc hl, de
	ex de, hl
	exx
1:

	ld a, (iy-4)
	jp p, 1f
	ld hl, 0			/* invert bc to make it positive */
	push hl
	exx
	pop hl
	exx
	and a
	sbc hl, bc
	push hl
	pop bc
	exx
	sbc hl, bc
	push hl
	pop bc
	exx
1:

	call .rmudvu6		/* actually do the division */

	ld a, (iy-4)
	xor (iy-1)			/* discover sign of remainder */
	jp p, 1f
	push hl				/* invert remainder if it should be negative */
	pop bc
	exx
	push hl
	pop bc
	exx
	ld hl, 0
	push hl
	exx
	pop hl
	exx
	and a
	sbc hl, bc
	exx
	sbc hl, bc
	exx
1:

	pop af
	pop af
	pop iy
	pop af
	ret

