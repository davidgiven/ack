#include "em_abs.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define	.setx

        ! $Id$


	! bytes in r0
.setx:
	movl	(sp)+,r3
	movl	(sp)+,r1	! bitnumber in r1
	subl3	r0,sp,r2
I1:
	clrl	-(sp)
	cmpl	sp,r2
	bgtru	I1
	ashl	$3,r0,r2	! number of bits in r2
	cmpl	r1,r2
	bgequ	I2
	bbcs	r1,(sp),I3
I3:
	jmp	(r3)
I2:
	pushl	$ESET
	jsb	.trp
	jmp	(r3)
