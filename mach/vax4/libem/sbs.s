        # $Header$
.globl .sbs

.sbs:
	movl    (sp)+,r1
	subl2   (sp)+,(sp)
	movl    $4,-(sp)
	movl    r0,-(sp)
	movl    r1,-(sp)
	jmp     .cii
