#include "em_abs.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define .cfu

        ! $Id$


.cfu:
	movl    (sp)+,r2
	movpsl  r1
	bicl2   $~040,r1
	bicpsw  $040            ! integer overflow traps must be ignored
	movl    (sp)+,r0
	cmpl    (sp),$4
	bneq    1f
	tstl    (sp)+
	tstf    (sp)
	bgeq    5f
	mnegf   (sp),(sp)
5:
	cvtfl   (sp)+,-(sp)
2:
	cmpl    r0,$4
	bneq    4f
	bispsw  r1              ! restore trap enable bit
	jmp     (r2)
1:
	cmpl    (sp)+,$8
	bneq    4f
	tstd    (sp)
	bgeq    3f
	mnegd   (sp),(sp)
3:
	cvtdl   (sp)+,-(sp)
	br      2b
4:
	pushl	$EILLINS
	jmp     .fat
