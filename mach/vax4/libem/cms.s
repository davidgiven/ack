.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define	.cms

        ! $Id$

	! bytes in r0
.cms:
	movl	(sp)+,r3
	addl3	sp,r0,r1
	addl3	r1,r0,r2
	ashl	$-2,r0,r0
I1:
	cmpl	(sp)+,(r1)+
	bneq	I2
	sobgtr	r0,I1
I2:
	movl	r2,sp
	jmp	(r3)
