#include "em_abs.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define .sts

        ! $Id$


.sts:
	movl	(sp)+,r2
	cmpl	r0,$4
	beql	I1
	pushl	$EILLINS
	jmp	.fat
I1:
	movl	(sp)+,r0	! number of bytes in r0
	movl	(sp)+,r1	! address in r1
	cmpl	r0,$1
	beql	I3
	cmpl	r0,$2
	beql	I4
	ashl	$-2,r0,r0
I2:
	movl	(sp)+,(r1)+
	sobgtr	r0,I2
	jmp	(r2)
I3:
	movl	(sp)+,r0
	movb	r0,(r1)
	jmp	(r2)
I4:
	movl	(sp)+,r0
	movw	r0,(r1)
	jmp	(r2)
