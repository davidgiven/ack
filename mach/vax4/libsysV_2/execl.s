.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define	_execl

_execl:
	.data2	0x0000
	pushab	8(ap)
	pushl	4(ap)
	calls	$2,_execv
	ret
