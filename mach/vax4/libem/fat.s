.sect .text; .sect .rom; .sect .data; .sect .bss; .sect .text
.define	.fat

        ! $Header$

.fat:
	pushl	(sp)
	jsb	.trp
	movl	$I1,ap
	movl	(sp)+,6(ap)
	chmk	(ap)+
	halt

	.sect .rom
I1:	.data2	1
	.data4	1
	.data4	0
