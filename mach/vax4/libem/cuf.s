#include "em_abs.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define .cuf

        ! $Header$


.cuf:
	movl    (sp)+,r2
	movl    (sp)+,r0
	movl    (sp)+,r1
	cmpl    r1,$4
	bneq    Ierr
	cmpl    r0,$4
	bneq    Idld
	cvtlf   (sp)+,-(sp)
	bgeq    Iout
	addf2	Itwo32f,(sp)
Iout:
	jmp     (r2)
Idld:
	cmpl    r0,$8
	bneq    Ierr
	cvtld   (sp)+,-(sp)
	bgeq    Iout
	addd2   Itwo32F,(sp)
	jmp     (r2)
Ierr:
	pushl	$EILLINS
	jmp     .fat
.sect .rom
Itwo32f:
	.data1 0200,0117,00,00
Itwo32F:
	.data1 0200,0117,00,00,00,00,00,00
