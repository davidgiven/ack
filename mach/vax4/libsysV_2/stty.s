.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
stty = 31
.define	_stty

_stty:
	.data2	0x0000
	chmk	$stty
	bcc	1f
	jmp	cerror
1:
	ret
