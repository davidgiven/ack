#
#include "asm.h"

! Creates a singleton set.
! Stack: ( bitnumber size -- set )

.sect .text
.define .set
.set:
	pop bc				! return address
	pop de				! size
	pop hl				! bit number

	push de
	pop ix				! size into ix

	! Push enough zeroes for the bitset.

	exx
	ld de, 0
	ld bc, -3
1:
	push de
	add ix, bc
	jr nz, 1b
	exx
	push bc				! set up return stack frame

	! Calculate the byte mask.

	ld a, l
	and ~7
	ld b, a
	ld a, 1
1:
	add a, a
	djnz 1b

	! Calculate the byte offset.

	push hl
	ld ix, 0
	add ix, sp
	ld b, 3
1:
	srl (ix+2)
	rr (ix+1)
	rr (ix+0)
	djnz 1b
	pop hl

	ld bc, 3
	add hl, bc
	add hl, sp			! HL is pointer to byte

	! Mask it

	or (hl)
	ld (hl), a

	ret


