#
#include "asm.h"

/* Divides two sixteen-bit unsigned numbers: DE = DE / BC
 * The quotient is returned in DE, the remainder in HL.
 */

.sect .text
.define .rmudvu3
.rmudvu3:
	ld a, 24			/* bit count */
	ld hl, 0			/* reset remainder */
	jr entry
loop1:
	add hl, de
loop2:
	dec a
	ret z
entry:
	ex de, hl			/* left shift LHS */
	add hl, hl
	ex de, hl

	adc hl, hl			/* top bit of LHS goes into remainder */
	and a				/* clear carry */
	sbc hl, bc			/* sets carry if remainder(hl) < bc */
	jr c, loop1			/* if remainder < bc, undo add and go round again */
	inc e				/* we know the bottom bit is clear */
	jr loop2

