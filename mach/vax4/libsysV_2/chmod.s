.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
chmod = 15
.define	_chmod

_chmod:
	.data2	0x0000
	chmk	$chmod
	bcc 	1f
	jmp 	cerror
1:
	clrl	r0
	ret
