.set	stat,18
.globl	_stat
.globl  cerror

_stat:
	.word	0x0000
	chmk	$stat
	bcc 	ok
	jmp 	cerror
ok:
	clrl	r0
	ret
