.globl	_acct
.set	acct,51

.align	1
_acct:
	.word	0x0000
	chmk	$acct
	bcc 	1f
	jmp 	errmon
1:
	ret
