.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define .sbs

        ! $Id$

.sbs:
	movl    (sp)+,r1
	subl2   (sp)+,(sp)
	movl    $4,-(sp)
	movl    r0,-(sp)
	movl    r1,-(sp)
	jmp     .cii
