.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define	_setgid
setgid = 46

.align	1
_setgid:
	.data2	0x0000
	chmk	$setgid
	bcc 	1f
	jmp 	errmon
1:
	clrl	r0
	ret
