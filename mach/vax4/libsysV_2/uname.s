.set	utssys,57

.globl	_uname
.globl	cerror

_uname:
	.word	0x0000
	pushl	$0
	pushl	$0
	pushl	4(ap)
	calls	$3,sys
	ret

sys:
	.word	0x0000
	chmk	$utssys
	bcc	ok
	jmp	cerror
ok:
	clrl	r0
	ret
