.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define  .aar4

        ! $Id$

.aar4:
	movl    (sp)+,r2
	movl    (sp)+,r0
	movl	8(r0),r1	! elementsize in r1
	subl3   (r0),(sp)+,r0
	mull2	r1,r0
	addl2	(sp)+,r0
	jmp     (r2)
