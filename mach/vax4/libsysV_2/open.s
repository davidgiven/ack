.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
open = 5
.define	_open

_open:
	.data2	0x0000
	chmk	$open
	bcc 	1f
	jmp 	cerror
1:
	ret
