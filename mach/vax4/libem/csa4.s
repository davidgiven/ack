#include "em_abs.h"

        # $Header$

.globl  .csa4

.csa4:
	movl    (sp)+,r0        # descriptor address
	movl    (sp)+,r1        # index
	movl    (r0)+,r2        # default
	subl2   (r0)+,r1
	cmpl    (r0)+,r1
	blssu   L1
	movl    (r0)[r1],r1
	bneq    L2
L1:
	movl    r2,r1
	bneq    L2
	pushl	$ECASE
	jmp     .fat
L2:
	jmp     (r1)
