#include "em_abs.h"

        # $Header$

.globl .cmf

.cmf:
	movl    (sp)+,r1
	cmpl    r0,$4
	bneq    L1
	clrl    r0
	cmpf    (sp)+,(sp)+
	brb     L3
L1:
	cmpl    r0,$8
	bneq    Lerr
	clrl    r0
	cmpd    (sp)+,(sp)+
L3:
	beql    L2
	bgtr    L4
	incl    r0
	brb     L2
L4:
	decl    r0
L2:
	jmp     (r1)
Lerr:
	pushl	$EILLINS
	jmp     .fat
