.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
mpxcall = 56
.define	_mpxcall

.align	1
_mpxcall:
	.data2	0x0000
	chmk	$mpxcall
	bcc 	1f
	jmp 	errmon
1:
	ret
