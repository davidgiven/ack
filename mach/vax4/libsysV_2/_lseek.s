.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
lseek = 19
.define	__lseek

__lseek:
	.data2	0x0000
	chmk	$lseek
	bcc 	1f
	jmp 	cerror
1:
	ret
