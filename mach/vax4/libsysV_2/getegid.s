.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
getgid = 47
.define	_getegid

_getegid:
	.data2	0x0000
	chmk	$getgid
	movl	r1,r0
	ret
