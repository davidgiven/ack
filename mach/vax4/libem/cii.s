        # $Header$
#include "em_abs.h"

.globl  .cii

.cii:
	movl    (sp)+, r1
	movl	(sp)+, r0
	cmpl	(sp), $1
	beql	Lfrom1
	cmpl	(sp), $2
	beql	Lfrom2
	cmpl	(sp)+, $4
	bgtr	Lerr
	jmp	(r1)
Lfrom1:
	cvtbw	4(sp), 4(sp)
Lfrom2:
	tstl	(sp)+
	cvtwl	(sp), (sp)
	jmp	(r1)
Lerr:
	pushl	$EILLINS
	jmp     .fat
