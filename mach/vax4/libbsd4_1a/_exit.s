.globl	__exit
.set	exit,1

.align	1
__exit:
	.word	0x0000
	chmk	$exit
	halt
