.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define	_link
link = 9

.align	1
_link:
	.data2	0x0000
	chmk	$link
	bcc 	1f
	jmp 	errmon
1:
	clrl	r0
	ret
