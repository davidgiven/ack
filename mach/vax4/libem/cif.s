#include "em_abs.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define  .cif

        ! $Id$


.cif:
	movl    (sp)+,r1
	movl    (sp)+,r0
	cmpl    r0,$4
	bneq    Idbl
	movl    (sp)+,r0
	cmpl    r0,$4
	bneq    Ierr
	cvtlf   (sp)+,-(sp)
	jmp     (r1)
Idbl:
	cmpl    r0,$8
	bneq    Ierr
	movl    (sp)+,r0
	cmpl    r0,$4
	bneq    Ierr
	cvtld   (sp)+,-(sp)
	jmp     (r1)
Ierr:
	pushl	$EILLINS
	jmp     .fat
