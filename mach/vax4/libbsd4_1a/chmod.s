.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define	_chmod
chmod = 15

.align	1
_chmod:
	.data2	0x0000
	chmk	$chmod
	bcc 	1f
	jmp 	errmon
1:
	clrl	r0
	ret
