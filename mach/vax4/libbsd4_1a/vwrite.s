.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define	_vwrite
vwrite = 68

.align	1
_vwrite:
	.data2	0x0000
	chmk	$vwrite
	bcc 	1f
	jmp 	errmon
1:
	ret
