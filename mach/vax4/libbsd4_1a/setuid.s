.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define	_setuid
setuid = 23

.align	1
_setuid:
	.data2	0x0000
	chmk	$setuid
	bcc 	1f
	jmp 	errmon
1:
	clrl	r0
	ret
