.set	exece,59
.globl	_execve
.globl	cerror

_execve:
	.word	0x0000
	chmk	$exece
	jmp 	cerror
