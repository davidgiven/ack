.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define	_getgid
.define	_getegid
getgid = 47

.align	1
_getgid:
	.data2	0x0000
	chmk	$getgid
	ret

.align	1
_getegid:
	.data2	0x0000
	chmk	$getgid
	movl	r1,r0
	ret
