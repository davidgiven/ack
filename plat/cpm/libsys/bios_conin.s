#
#include "asm.h"

.define _bios_conin
_bios_conin:
	lhld 1
	lxi d, 6
	dad d
	push b
	call .pchl
	pop b
	mov e, a
	mvi d, 0
	ret

