.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
gtty = 32
.define	__gtty

__gtty:
	.data2	0x0000
	chmk	$gtty
	bcc	1f
	jmp	cerror
1:
	ret
