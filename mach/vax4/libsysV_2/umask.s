.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
umask = 60
.define	_umask

_umask:
	.data2	0x0000
	chmk	$umask
	bcc	1f
	jmp	cerror
1:
	ret
