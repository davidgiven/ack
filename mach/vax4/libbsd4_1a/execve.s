.globl	_execve
.set	exec,59

.align	1
_execve:
	.word	0x0000
	chmk	$exec
	jmp 	errmon
