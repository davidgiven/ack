.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define	_nice
nice = 34

.align	1
_nice:
	.data2	0x0000
	chmk	$nice
	bcc 	1f
	jmp 	errmon
1:
	clrl	r0
	ret
