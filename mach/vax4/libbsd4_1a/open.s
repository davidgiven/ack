.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define	_open
open = 5

.align	1
_open:
	.data2	0x0000
	chmk	$open
	bcc 	1f
	jmp 	errmon
1:
	ret
