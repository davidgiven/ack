.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define	.inn

        ! $Id$

	! bytes in r0
.inn:
	movl	(sp)+,r3
	movl	(sp)+,r1
	ashl	$3,r0,r2	! nr of bits in r2
	cmpl	r1,r2
	bgequ	I2
	bbc	r1,(sp),I2
	movl	$1,r1
I1:
	addl2	r0,sp
	jmp	(r3)
I2:
	clrl	r1
	br	I1
