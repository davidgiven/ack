.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define	_chdir
chdir = 12

.align	1
_chdir:
	.data2	0x0000
	chmk	$chdir
	bcc 	1f
	jmp 	errmon
1:
	clrl	r0
	ret
