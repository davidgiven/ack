.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define	.exg

        ! $Id$

	! bytes in r0
.exg:
	movl	(sp)+,r3
	addl3	sp,r0,r2
	addl2	r0,r2
	ashl	$-2,r0,r1
I1:
	movl	-(r2),-(sp)
	sobgtr	r1,I1
	addl3	r0,r2,r1
I2:
	movw	-(r2),-(r1)
	sobgtr	r0,I2
	movl	r1,sp
	jmp	(r3)
