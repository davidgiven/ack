.set	utssys,57
.globl	_ustat
.globl	cerror

_ustat:
	.word	0x0000
	pushl	$2
	pushl	4(ap)
	pushl	8(ap)
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
