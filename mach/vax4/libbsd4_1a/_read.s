.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define	__read
read = 3

.align	1
__read:
	.data2	0x0000
	chmk	$read
	bcc 	1f
	jmp 	errmon
1:
	ret
