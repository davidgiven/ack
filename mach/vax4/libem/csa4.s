#include "em_abs.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define  .csa4

        ! $Id$


.csa4:
	movl    (sp)+,r0        ! descriptor address
	movl    (sp)+,r1        ! index
	movl    (r0)+,r2        ! default
	subl2   (r0)+,r1
	cmpl    (r0)+,r1
	blssu   I1
	movl    (r0)[r1],r1
	bneq    I2
I1:
	movl    r2,r1
	bneq    I2
	pushl	$ECASE
	jmp     .fat
I2:
	jmp     (r1)
