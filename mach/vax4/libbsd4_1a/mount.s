.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define	_mount
mount = 21

.align	1
_mount:
	.data2	0x0000
	chmk	$mount
	bcc 	1f
	jmp 	errmon
1:
	clrl	r0
	ret
