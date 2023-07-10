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
	or a
	jp p, 1f

	ld hl, 0			/* invert DEDE to make it positive */
	push hl
	and a
	sbc hl, de
	ex de, hl
	exx
	pop hl
	sbc hl, de
	ex de, hl
	exx
1:

	ld a, (iy-4)
	or a
	jp p, 1f

	ld hl, 0			/* invert BCBC to make it positive */
	push hl
	and a
	sbc hl, bc
	push hl
	pop bc
	exx
	pop hl
	sbc hl, bc
	push hl
	pop bc
	exx
1:

	call .rmudvu6		/* actually do the division */

	ld a, (iy-4)
	xor (iy-1)			/* discover sign of result */
	jp p, 1f

	push hl
	ld hl, 0			/* invert result if it should be negative */
	exx
	push hl
	ld hl, 0
	exx

	and a
	sbc hl, de
	ex de, hl
	pop hl
	exx
	sbc hl, de
	ex de, hl
	pop hl
	exx
1:

	ld a, (iy-1)		/* discover sign of remainder */
	or a
	jp p, 1f

	push hl				/* invert remainder if it should be negative */
	pop bc
	ld hl, 0
	exx
	push hl
	pop bc
	ld hl, 0
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

