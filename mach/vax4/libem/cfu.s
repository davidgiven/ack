#include "em_abs.h"

        # $Header$

.globl .cfu

.cfu:
	movl    (sp)+,r2
	movpsl  r1
	bicl2   $~040,r1
	bicpsw  $040            # integer overflow traps must be ignored
	movl    (sp)+,r0
	cmpl    (sp),$4
	bneq    Lddt
	tstl    (sp)+
	tstf    (sp)
	bgeq    L1
	mnegf   (sp),(sp)
L1:
	cvtfl   (sp)+,-(sp)
L2:
	cmpl    r0,$4
	bneq    Lerr
	bispsw  r1              # restore trap enable bit
	jmp     (r2)
Lddt:
	cmpl    (sp)+,$8
	bneq    Lerr
	tstd    (sp)
	bgeq    L3
	mnegd   (sp),(sp)
L3:
	cvtdl   (sp)+,-(sp)
	brb     L2
Lerr:
	pushl	$EILLINS
	jmp     .fat
