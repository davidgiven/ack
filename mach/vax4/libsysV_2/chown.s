.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
chown = 16
.define	_chown

_chown:
	.data2	0x0000
	chmk	$chown
	bcc 	1f
	jmp 	cerror
1:
	clrl	r0
	ret
