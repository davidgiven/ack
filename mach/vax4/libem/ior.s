.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define	.ior

        ! $Id$

	! bytes in r0
.ior:
	movl	(sp)+,r2
	addl3	r0,sp,r1
	ashl	$-2,r0,r0
I1:
	bisl2	(sp)+,(r1)+
	sobgtr	r0,I1
	jmp	(r2)
