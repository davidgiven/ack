.set	ptrace,26
.globl	_ptrace
.globl  _errno
.globl	cerror

_ptrace:
	.word	0x0000
	clrl	_errno
	chmk	$ptrace
	bcc 	ok
	jmp 	cerror
ok:
	ret
