.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define	_chown
chown = 16

.align	1
_chown:
	.data2	0x0000
	chmk	$chown
	bcc 	1f
	jmp 	errmon
1:
	clrl	r0
	ret
