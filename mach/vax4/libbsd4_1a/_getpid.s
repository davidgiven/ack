.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define	__getpid
.define	__getppid
getpid = 20

.align	1
__getpid:
	.data2	0x0000
	chmk	$getpid
	ret

.align	1
__getppid:
	.data2	0x0000
	chmk	$getpid
	movl	r1,r0
	ret
