.set	acct,51
.globl	_acct
.globl  cerror

_acct:
	.word	0x0000
	chmk	$acct
	bcc 	ok
	jmp 	cerror
ok:
	ret
