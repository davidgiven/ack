.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define	_read
read = 3

.align	1
_read:
	.data2	0x0000
	chmk	$read
	bcc 	1f
	jmp 	errmon
1:
	ret
