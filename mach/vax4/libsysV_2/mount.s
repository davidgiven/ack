.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
mount = 21
.define	_mount

_mount:
	.data2	0x0000
	chmk	$mount
	bcc	1f
	jmp 	cerror
1:
	clrl	r0
	ret
