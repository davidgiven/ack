#include "em_abs.h"

        # $Header$

.globl .bls

.bls:
	movl	(sp)+,r3
	cmpl	r0,$4
	bneq	Lerr
	movl	(sp)+,r0	# number of bytes in r0
	movl	(sp)+,r1	# addresses in r1, r2
	movl	(sp)+,r2
	blbc	r0,L1
	movb	(r2)+,(r1)+
L1:
	bbc	$1,r0,L2
	movw	(r2)+,(r1)+
L2:
	ashl	$-2,r0,r0
	beql	L4
L3:
	movl	(r2)+,(r1)+
	sobgtr	r0,L3
L4:
	jmp	(r3)
Lerr:
	pushl	$EILLINS
	jmp	.fat
