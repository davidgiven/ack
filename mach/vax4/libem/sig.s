.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define  .sig

        ! $Id$

.sig:
	movl    (sp)+,r1
	movl    (sp)+,r0
	pushl	.trppc
	movl    r0,.trppc
	jmp     (r1)
