.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define	_lseek
lseek = 19

.align	1
_lseek:
	.data2	0x0000
	chmk	$lseek
	bcc 	1f
	jmp 	errmon
1:
	ret
