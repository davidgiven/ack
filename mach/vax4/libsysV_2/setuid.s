.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
setuid = 23
.define	_setuid

_setuid:
	.data2	0x0000
	chmk	$setuid
	bcc 	1f
	jmp 	cerror
1:
	clrl	r0
	ret
