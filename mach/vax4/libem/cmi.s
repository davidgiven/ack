#include "em_abs.h"

        # $Header$

.globl .cmi

.cmi:
	movl    (sp)+,r1
	cmpl    r0,$4
	bneq    Lerr
	clrl    r0
	cmpl    (sp)+,(sp)+
	beql    L1
	bgtr    L2
	incl    r0
	brb     L1
L2:
	decl    r0
L1:
	jmp     (r1)
Lerr:
	pushl	$EILLINS
	jmp     .fat
