.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define	_access
access = 33

.align	1
_access:
	.data2	0x0000
	chmk	$access
	bcc 	1f
	jmp 	errmon
1:
	ret
