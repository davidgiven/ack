.set	execve,59
.globl	_exect
.globl	cerror

_exect:
	.word	0x0000
	bispsw	$0x10
	chmk	$execve
	jmp 	cerror
