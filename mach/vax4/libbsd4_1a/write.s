.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define	_write
write = 4

.align	1
_write:
	.data2	0x0000
	chmk	$write
	bcc	1f
	jmp	errmon
1:
	ret
