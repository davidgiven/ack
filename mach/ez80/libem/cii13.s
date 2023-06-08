#
#include "asm.h"

.sect .text
.define .cii13
.cii13:
	or a			/* set m flag */
	ld hl, 0
	jp p, exit

	dec hl			/* negative */
exit:
	ld l, a
	ret



