.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
.define	_ptrace
ptrace = 26

.align	1
_ptrace:
	.data2	0x0000
	clrl	_errno
	chmk	$ptrace
	bcc 	1f
	jmp 	errmon
1:
	ret
