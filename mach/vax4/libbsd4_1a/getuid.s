.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define	_getuid
.define	_geteuid
getuid = 24

.align	1
_getuid:
	.data2	0x0000
	chmk	$getuid
	ret

.align	1
_geteuid:
	.data2	0x0000
	chmk	$getuid
	movl	r1,r0
	ret
