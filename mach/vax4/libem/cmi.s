        # $Header$
#include "em_abs.h"

.globl .cmi

.cmi:
	movl    (sp)+,r1
	cmpl    r0,$4
	bneq    err
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
err:
	pushl	$EILLINS
	jmp     .fat
