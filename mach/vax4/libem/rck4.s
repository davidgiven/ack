#include "em_abs.h"

        # $Header$

.globl .rck4

.rck4:
	movl	(sp)+,r1
	movl	(sp)+,r0
	cmpl	(sp),(r0)	# compare lower bound
	blss	Lerr
	cmpl	(sp),4(r0)	# compare upper bound
	bgtr	Lerr
	jmp	(r1)
Lerr:
	pushl	r1
	pushl	$ERANGE
	jsb	.trp
	rsb
