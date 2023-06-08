#
#include "asm.h"

/* Divides two sixteen-bit unsigned numbers: DE = DE / BC
 * The quotient is returned in DE, the remainder in HL.
 */

.sect .text
.define .rmudvu6
.rmudvu6:
	ld a, 24			/* bit count */
	ld hl, 0			/* reset remainder */
	exx
	ld hl, 0
	exx
	jr entry
loop1:
	add hl, de
	exx
	adc hl, de
	exx
loop2:
	dec a
	ret z
entry:
	ex de, hl			/* left shift LHS */
	add hl, hl
	ex de, hl
	exx
	ex de, hl
	adc hl, hl
	ex de, hl
	exx

	adc hl, hl			/* top bit of LHS goes into remainder */
	exx
	adc hl, hl
	exx

	and a				/* clear carry */
	sbc hl, bc			/* sets carry if remainder(hl) < bc */
	exx
	sbc hl, bc
	exx

	jr c, loop1			/* if remainder < bc, undo add and go round again */
	inc e				/* we know the bottom bit is clear */
	jr loop2

