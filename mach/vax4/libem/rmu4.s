.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define	.rmu4

        ! $Id$

.rmu4:
	movl    (sp)+,r3
	movl    (sp)+,r2
	blss    I1
	movl    (sp)+,r0
	clrl    r1
	ediv    r2,r0,r1,r0
	jmp     (r3)
I1:
	movl    (sp)+,r0
	cmpl    r0,r2
	blssu   I2
	subl2   r2,r0
I2:
	jmp     (r3)
