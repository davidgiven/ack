#include "em_abs.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define .cmf

        ! $Id$


.cmf:
	movl    (sp)+,r1
	cmpl    r0,$4
	bneq    I1
	clrl    r0
	cmpf    (sp)+,(sp)+
	br     I3
I1:
	cmpl    r0,$8
	bneq    Ierr
	clrl    r0
	cmpd    (sp)+,(sp)+
I3:
	beql    I2
	bgtr    I4
	incl    r0
	br     I2
I4:
	decl    r0
I2:
	jmp     (r1)
Ierr:
	pushl	$EILLINS
	jmp     .fat
