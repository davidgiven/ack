#
#include "asm.h"

.sect .text
.define .extendb
.extendb:
	or a			/* set m flag */
	ld hl, 0
	jp p, exit

	dec hl			/* negative */
exit:
	ld l, a
	ret



