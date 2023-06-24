#
#include "asm.h"

.sect .text

! Tests a bit in a bitset on the stack.
! 
! Stack: ( bitset bitnum setsize -- bool )

.define .inn
.inn:
	push iy
	ld iy, 0
	add iy, sp

	lea ix, iy+9	! ix points at bit number
	ld a, (ix+0)
	and ~7			! get bit number within byte

	ld b, 3			! shift bit number to get byte offset
1:
	srl (ix+2)
	rr (ix+1)
	rr (ix+0)
	djnz 1b

	ld b, a			! bit within byte
	lea hl, iy+12	! hl points at bitset
	ld de, (iy+9)	! byte-masked bit number
	add hl, de
	ld a, (hl)		! get byte

	inc b
1:
	rra
	djnz 1b

	! The bit we want is now in C.

	ld bc, 0
	rl c			! de is now a boolean result

	lea hl, iy+12	! hl points at bitset
	ld de, (iy+6)	! size of bitset
	add hl, de		! top of bitset --- new stack pointer

	pop iy
	pop de			! return address

	ld sp, hl		! retract
	push bc			! return value
	ex de, hl
	jp (hl)			! return

