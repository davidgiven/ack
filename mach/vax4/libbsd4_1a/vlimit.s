.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define	_vlimit
vlimit = 77

.align	1
_vlimit:
	.data2	0x0000
	chmk	$vlimit
	bcc 	1f
	jmp 	errmon
1:
	ret
