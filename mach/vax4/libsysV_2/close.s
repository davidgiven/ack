.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
close = 6
.define	_close

_close:
	.data2	0x0000
	chmk	$close
	bcc 	1f
	jmp 	cerror
1:
	clrl	r0
	ret
