.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
ulimit = 63
.define	_ulimit

_ulimit:
	.data2	0x0000
	chmk	$ulimit
	bcc	1f
	jmp	cerror
1:
	ret
