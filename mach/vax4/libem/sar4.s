        # $Header$
.globl  .sar4

.sar4:
	movl    (sp)+,r2
	movl    (sp)+,r0
	movl	8(r0),r1
	subl2   (r0),(sp)
	movl	(sp)+,r0
	mull2	r1,r0
	addl2   (sp)+,r0
	cmpl	r1,$1
	bgtr	L3
	cvtlb   (sp)+,(r0)
	jmp     (r2)
L3:
	cmpl	r1,$2
	bgtr	L2
	cvtlw	(sp)+,(r0)
	jmp	(r2)
L2:
	ashl    $-2,r1,r1
L1:
	movl    (sp)+,(r0)+
	sobgtr  r1,L1
	jmp     (r2)
