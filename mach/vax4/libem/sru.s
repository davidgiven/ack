#include "em_abs.h"

        # $Header$

.globl .sru

.sru:
	movl    (sp)+,r3
	cmpl    r0,$4
	beql    .sru4
	pushl	$EILLINS
	jmp     .fat
.sru4:
	movl    (sp)+,r2
	mnegl   r2,r2
	movl    (sp)+,r0
	clrl    r1
	ashq    r2,r0,r0
	movl    r0,-(sp)
	jmp     (r3)
