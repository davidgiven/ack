.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define  .sar4

        ! $Id$

.sar4:
	movl    (sp)+,r2
	movl    (sp)+,r0
	movl	8(r0),r1
	subl2   (r0),(sp)
	movl	(sp)+,r0
	mull2	r1,r0
	addl2   (sp)+,r0
	cmpl	r1,$1
	bgtr	I3
	cvtlb   (sp)+,(r0)
	jmp     (r2)
I3:
	cmpl	r1,$2
	bgtr	I2
	cvtlw	(sp)+,(r0)
	jmp	(r2)
I2:
	ashl    $-2,r1,r1
I1:
	movl    (sp)+,(r0)+
	sobgtr  r1,I1
	jmp     (r2)
