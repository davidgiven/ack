.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define	_close
close = 6

.align	1
_close:
	.data2	0x0000
	chmk	$close
	bcc 	1f
	jmp 	errmon
1:
	clrl	r0
	ret
