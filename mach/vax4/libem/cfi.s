#include "em_abs.h"

        # $Header$

.globl  .cfi

.cfi:
	movl    (sp)+,r1
	movl    (sp)+,r0
	cmpl    r0,$4
	bneq    Lerr
	movl    (sp)+,r0
	cmpl    r0,$4
	bneq    Lddl
	cvtfl	(sp)+,-(sp)
	jmp     (r1)
Lddl:
	cmpl    r0,$8
	bneq    Lerr
	cvtdl	(sp)+,-(sp)
	jmp     (r1)
Lerr:
	pushl	$EILLINS
	jmp     .fat
