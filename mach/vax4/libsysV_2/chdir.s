.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
chdir = 12
.define	_chdir
.define	cerror

_chdir:
	.data2	0x0000
	chmk	$chdir
	bcc 	1f
	jmp 	cerror
1:
	clrl	r0
	ret
