#include "em_abs.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define .cff

        ! $Id$


.cff:
	movl    (sp)+,r1
	movl    (sp)+,r0
	cmpl    r0,$4
	bneq    1f
	movl    (sp)+,r0
	cmpl    r0,$4
	bneq    2f
	jmp     (r1)
2:
	cmpl    r0,$8
	bneq    4f
	cvtdf   (sp)+,-(sp)
	jmp     (r1)
1:
	movl    (sp)+,r0
	cmpl    r0,$4
	bneq    3f
	cvtfd   (sp)+,-(sp)
	jmp     (r1)
3:
	cmpl    r0,$8
	bneq    4f
	jmp     (r1)
4:
	pushl	$EILLINS
	jmp     .fat
