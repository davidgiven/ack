.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define	_vread
vread = 67

.align	1
_vread:
	.data2	0x0000
	chmk	$vread
	bcc 	1f
	jmp 	errmon
1:
	ret
