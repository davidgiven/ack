.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define	_execl
.define	_environ

.align	1
_execl:
	.data2	0x0000
	pushl	_environ
	pushab	8(ap)
	pushl	4(ap)
	calls	$3,_execve
	ret
