.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
unlink = 10
.define	_unlink

_unlink:
	.data2	0x0000
	chmk	$unlink
	bcc 	1f
	jmp 	cerror
1:
	clrl	r0
	ret
