#include "em_abs.h"

        # $Header$

.globl .sts

.sts:
	movl	(sp)+,r2
	cmpl	r0,$4
	beql	L1
	pushl	$EILLINS
	jmp	.fat
L1:
	movl	(sp)+,r0	# number of bytes in r0
	movl	(sp)+,r1	# address in r1
	cmpl	r0,$1
	beql	L3
	cmpl	r0,$2
	beql	L4
	ashl	$-2,r0,r0
L2:
	movl	(sp)+,(r1)+
	sobgtr	r0,L2
	jmp	(r2)
L3:
	movl	(sp)+,r0
	movb	r0,(r1)
	jmp	(r2)
L4:
	movl	(sp)+,r0
	movw	r0,(r1)
	jmp	(r2)
