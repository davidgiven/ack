.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
fcntl = 62
.define	__fcntl

__fcntl:
	.data2	0x0000
	chmk	$fcntl
	bcc	1f
	jmp	cerror
1:
	ret
