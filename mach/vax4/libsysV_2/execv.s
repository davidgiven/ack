.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define	_execv

_execv:
	.data2	0x0000
	pushl	_environ
	pushl	8(ap)
	pushl	4(ap)
	calls	$3,_execve
	ret
