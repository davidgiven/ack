.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define	__open
open = 5

.align	1
__open:
	.data2	0x0000
	chmk	$open
	bcc 	1f
	jmp 	errmon
1:
	ret
