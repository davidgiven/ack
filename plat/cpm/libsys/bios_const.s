#
#include "asm.h"

.define _bios_const
_bios_const:
	lhld 1
	lxi d, 3
	dad d
	push b
	call .pchl
	pop b
	mov e, a
	mvi d, 0
	ret


