.set	stime,25
.globl	_stime
.globl  cerror

_stime:
	.word	0x0000
	movl	*4(ap),4(ap)
	chmk	$stime
	bcc 	ok
	jmp 	cerror
ok:
	clrl	r0
	ret
