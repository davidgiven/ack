.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
dup = 41
.define	__dup

__dup:
	.data2	0x0000
	chmk	$dup
	bcc 	1f
	jmp 	cerror
1:
	ret
