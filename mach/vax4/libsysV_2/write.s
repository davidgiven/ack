.set	write,4
.globl	_write
.globl  cerror

_write:
	.word	0x0000
	chmk	$write
	bcc 	out
	jmp 	cerror
out:
	ret
