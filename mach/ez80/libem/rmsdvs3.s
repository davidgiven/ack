#
#include "asm.h"

/* Divides two twenty-four-bit signed numbers: DE = DE / BC
 * The quotient is returned in DE, the remainder in HL.
 */

.sect .text
.define .rmsdvs3
.rmsdvs3:
	push af
	push iy
	ld iy, 0
	add iy, sp

	push de				/* lhs */
	push bc				/* rhs */

	ld a, (iy-1)
	or a
	jp p, 1f
	ld hl, 0			/* invert de to make it positive */
	and a
	sbc hl, de
	ex de, hl
1:

	ld a, (iy-4)
	or a
	jp p, 1f
	ld hl, 0			/* invert bc to make it positive */
	and a
	sbc hl, bc
	push hl
	pop bc
1:

	call .rmudvu3		/* actually do the division */

	ld a, (iy-4)
	xor (iy-1)			/* discover sign of result */
	jp p, 1f
	push hl
	ld hl, 0			/* invert result if it should be negative */
	and a
	sbc hl, de
	ex de, hl
	pop hl
1:

	ld a, (iy-1)
	or a				/* discover sign of remainder */
	jp p, 1f
	push hl				/* invert remainder if it should be negative */
	pop bc
	ld hl, 0
	and a
	sbc hl, bc
1:

	pop bc
	pop af
	pop iy
	pop af
	ret

