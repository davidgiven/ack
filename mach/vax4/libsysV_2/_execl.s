.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define	__execl

__execl:
	.data2	0x0000
	pushab	8(ap)
	pushl	4(ap)
	calls	$2,__execv
	ret
