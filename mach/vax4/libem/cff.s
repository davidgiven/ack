        # $Header$
#include "em_abs.h"

.globl .cff

.cff:
	movl    (sp)+,r1
	movl    (sp)+,r0
	cmpl    r0,$4
	bneq    Ldtd
	movl    (sp)+,r0
	cmpl    r0,$4
	bneq    Lddf
	jmp     (r1)
Lddf:
	cmpl    r0,$8
	bneq    Lerr
	cvtdf   (sp)+,-(sp)
	jmp     (r1)
Ldtd:
	movl    (sp)+,r0
	cmpl    r0,$4
	bneq    Lddd
	cvtfd   (sp)+,-(sp)
	jmp     (r1)
Lddd:
	cmpl    r0,$8
	bneq    Lerr
	jmp     (r1)
Lerr:
	pushl	$EILLINS
	jmp     .fat
