        # $Header$
.globl  .rmu4

.rmu4:
	movl    (sp)+,r3
	movl    (sp)+,r2
	blss    L1
	movl    (sp)+,r0
	clrl    r1
	ediv    r2,r0,r1,r0
	jmp     (r3)
L1:
	movl    (sp)+,r0
	cmpl    r0,r2
	blssu   L2
	subl2   r2,r0
L2:
	jmp     (r3)
