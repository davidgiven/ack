.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
getpid = 20
.define	_getppid

_getppid:
	.data2	0x0000
	chmk	$getpid
	movl	r1,r0
	ret
