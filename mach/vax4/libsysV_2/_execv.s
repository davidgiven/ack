.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define	__execv

__execv:
	.data2	0x0000
	pushl	_environ
	pushl	8(ap)
	pushl	4(ap)
	calls	$3,__execve
	ret
