        # $Header$
.globl  .lar4

.lar4:
	movl    (sp)+,r2
	movl    (sp)+,r0
	movl	8(r0),r1
	subl2   (r0),(sp)
	mull3	(sp)+,r1,r0
	addl2   (sp)+,r0
	addl2   r1,r0
	cmpl	r1,$1
	bgtr	L3
	movzbl  -(r0),-(sp)
	jmp     (r2)
L3:
	cmpl	r1,$2
	bgtr	L2
	movzwl	-(r0),-(sp)
	jmp	(r2)
L2:
	ashl    $-2,r1,r1
L1:
	movl    -(r0),-(sp)
	sobgtr  r1,L1
	jmp     (r2)
