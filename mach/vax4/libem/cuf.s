        # $Header$
#include "em_abs.h"

.globl .cuf

.cuf:
	movl    (sp)+,r2
	movl    (sp)+,r0
	movl    (sp)+,r1
	cmpl    r1,$4
	bneq    Lerr
	cmpl    r0,$4
	bneq    Ldld
	cvtlf   (sp)+,-(sp)
	bgeq    Lout
	addf2	Ltwo32f,(sp)
Lout:
	jmp     (r2)
Ldld:
	cmpl    r0,$8
	bneq    Lerr
	cvtld   (sp)+,-(sp)
	bgeq    Lout
	addd2   Ltwo32F,(sp)
	jmp     (r2)
Lerr:
	pushl	$EILLINS
	jmp     .fat
.data
Ltwo32f:
	.float	0f4294967296.0
Ltwo32F:
	.double	0f4294967296.0
