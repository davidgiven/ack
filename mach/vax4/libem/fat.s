        # $Header$
.globl	.fat

.fat:
	pushl	(sp)
	jsb	.trp
	movl	$L1,ap
	movl	(sp)+,6(ap)
	chmk	(ap)+
	halt

	.data
L1:	.word	1
	.long	1
	.long	0
