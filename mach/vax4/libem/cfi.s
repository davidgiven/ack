#include "em_abs.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define  .cfi

        ! $Id$


.cfi:
	movl    (sp)+,r1
	movl    (sp)+,r0
	cmpl    r0,$4
	bneq    1f
	movl    (sp)+,r0
	cmpl    r0,$4
	bneq    2f
	cvtfl	(sp)+,-(sp)
	jmp     (r1)
2:
	cmpl    r0,$8
	bneq    1f
	cvtdl	(sp)+,-(sp)
	jmp     (r1)
1:
	pushl	$EILLINS
	jmp     .fat
