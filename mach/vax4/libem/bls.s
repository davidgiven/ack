#include "em_abs.h"
.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define .bls

        ! $Id$


.bls:
	movl	(sp)+,r3
	cmpl	r0,$4
	bneq	5f
	movl	(sp)+,r0	! number of bytes in r0
	movl	(sp)+,r1	! addresses in r1, r2
	movl	(sp)+,r2
	blbc	r0,1f
	movb	(r2)+,(r1)+
1:
	bbc	$1,r0,2f
	movw	(r2)+,(r1)+
2:
	ashl	$-2,r0,r0
	beql	4f
3:
	movl	(r2)+,(r1)+
	sobgtr	r0,3b
4:
	jmp	(r3)
5:
	pushl	$EILLINS
	jmp	.fat
