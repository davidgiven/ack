.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define	.fat

        ! $Header$

.fat:
	pushl	(sp)
	jsb	.trp
	movl	(sp)+,r0
	jmp	EXIT
