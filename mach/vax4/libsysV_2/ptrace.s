.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
ptrace = 26
.define	_ptrace

_ptrace:
	.data2	0x0000
	clrl	_errno
	chmk	$ptrace
	bcc 	1f
	jmp 	cerror
1:
	ret
