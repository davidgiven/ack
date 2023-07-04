#
#include "asm.h"

! Extends HLHL.

.sect .text
.define .extendw
.extendw:
	push de

	ld de, 0
	and a
	adc hl, de		/* set z flag */

	exx
	ld hl, 0		/* high word */
	jp p, exit

	dec hl
exit:
	exx

	pop de
	ret

