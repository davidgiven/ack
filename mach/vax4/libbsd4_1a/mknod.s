.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define	_mknod
mknod = 14

.align	1
_mknod:
	.data2	0x0000
	chmk	$mknod
	bcc 	1f
	jmp 	errmon
1:
	clrl	r0
	ret
