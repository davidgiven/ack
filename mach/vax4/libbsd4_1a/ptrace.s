.globl	_ptrace
.set	ptrace,26

.align	1
_ptrace:
	.word	0x0000
	clrl	_errno
	chmk	$ptrace
	bcc 	1f
	jmp 	errmon
1:
	ret
