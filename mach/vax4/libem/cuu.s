#include "em_abs.h"

        # $Header$

.globl .cuu

.cuu:
	movl    (sp)+,r1
	movl    (sp)+,r0
	cmpl    (sp),$1
	beql    Lfrom1
	cmpl    (sp),$2
	beql    Lfrom2
	cmpl	(sp)+, $4
	bneq	Lerr
	jmp     (r1)
Lfrom1:
	movzbw	4(sp), 4(sp)
Lfrom2:
	tstl	(sp)+
	movzwl	(sp), (sp)
	jmp	(r1)
Lerr:
	pushl	$EILLINS
	jmp     .fat
