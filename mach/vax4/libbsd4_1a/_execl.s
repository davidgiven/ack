.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define	__execl
.define	_environ

.align	1
__execl:
	.data2	0x0000
	pushl	_environ
	pushab	8(ap)
	pushl	4(ap)
	calls	$3,__execve
	ret
