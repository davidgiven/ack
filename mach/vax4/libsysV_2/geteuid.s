.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
getuid = 24
.define	_geteuid

_geteuid:
	.data2	0x0000
	chmk	$getuid
	movl	r1,r0
	ret
