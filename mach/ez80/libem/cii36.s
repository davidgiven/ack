#
#include "asm.h"

.sect .text
.define .cii36
.cii36:
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

