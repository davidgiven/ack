.set	wait,7
.globl	_wait
.globl  cerror

_wait:
	.word	0x0000
	chmk	$wait
	bcc 	ok
	jmp 	cerror
ok:
	tstl	4(ap)
	beql	out
	movl	r1,*4(ap)
out:
	ret
