#include "em_abs.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define .rck4

        ! $Id$


.rck4:
	movl	(sp)+,r1
	movl	(sp)+,r0
	cmpl	(sp),(r0)	! compare lower bound
	blss	Ierr
	cmpl	(sp),4(r0)	! compare upper bound
	bgtr	Ierr
	jmp	(r1)
Ierr:
	pushl	r1
	pushl	$ERANGE
	jsb	.trp
	rsb
