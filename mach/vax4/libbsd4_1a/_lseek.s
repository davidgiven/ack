.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define	__lseek
lseek = 19

.align	1
__lseek:
	.data2	0x0000
	chmk	$lseek
	bcc 	1f
	jmp 	errmon
1:
	ret
