.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define	.fat

        ! $Id$

.fat:
	pushl	(sp)
	jsb	.trp
	movl	(sp)+,r0
	jmp	EXIT
