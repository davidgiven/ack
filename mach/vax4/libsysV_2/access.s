.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
access = 33
.define	_access
.define	cerror

_access:
	.data2	0x0000
	chmk	$access
	bcc 	1f
	jmp 	cerror
1:
	ret
