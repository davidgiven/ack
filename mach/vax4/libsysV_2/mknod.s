.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
mknod = 14
.define	_mknod

_mknod:
	.data2	0x0000
	chmk	$mknod
	bcc 	1f
	jmp 	cerror
1:
	clrl	r0
	ret
