.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define	_getpid
.define	_getppid
getpid = 20

.align	1
_getpid:
	.data2	0x0000
	chmk	$getpid
	ret

.align	1
_getppid:
	.data2	0x0000
	chmk	$getpid
	movl	r1,r0
	ret
