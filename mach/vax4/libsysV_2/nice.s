.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
nice = 34
.define	_nice

_nice:
	.data2	0x0000
	chmk	$nice
	bcc 	1f
	jmp 	cerror
1:
	ret
