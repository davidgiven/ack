.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
setgid = 46
.define	_setgid

_setgid:
	.data2	0x0000
	chmk	$setgid
	bcc 	1f
	jmp 	cerror
1:
	clrl	r0
	ret
