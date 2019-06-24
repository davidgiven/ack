#
#include "asm.h"

.define _bios_conout
_bios_conout:
	pop h			! pop return address
	pop d           ! pop parameter into DE
	push d
	push h
	
	push b
	mov c, e

	lhld 1
	lxi d, 9
	dad d
	call .pchl

	pop b
	ret

