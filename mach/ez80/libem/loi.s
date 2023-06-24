#
#include "asm.h"

! Performs a variable-sized load.
! Stack is: ( ptr size : result )

.sect .text
.define .loi
.loi:
	pop hl				! return address
	exx
	pop bc				! object size
	pop ix				! pointer

	add ix, bc			! go to top of object
	push bc
	pop hl				! size into HL

	! loop in reverse
1:
	ld de, (ix-3)
	push de
	lea ix, ix-3

	ld de, -3
	add hl, de
	jr nz, 1b
	
	exx
	jp (hl)
