.set	exit,1
.globl	__exit

__exit:
	.word	0x0000
	chmk	$exit
	halt
