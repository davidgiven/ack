#include "em_abs.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define  .cii

        ! $Id$


.cii:
	movl    (sp)+, r1
	movl	(sp)+, r0
	cmpl	(sp), $1
	beql	Ifrom1
	cmpl	(sp), $2
	beql	Ifrom2
	cmpl	(sp)+, $4
	bgtr	Ierr
	jmp	(r1)
Ifrom1:
	cvtbw	4(sp), 4(sp)
Ifrom2:
	tstl	(sp)+
	cvtwl	(sp), (sp)
	jmp	(r1)
Ierr:
	pushl	$EILLINS
	jmp     .fat
