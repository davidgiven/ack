.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define	_umask
umask = 60

.align	1
_umask:
	.data2	0x0000
	chmk	$umask
	bcc	1f
	jmp	errmon
1:
	ret
