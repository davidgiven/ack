.set	msg,49
.globl	_msgrcv
.globl	_msgctl
.globl	_msgsnd
.globl	_msgget
.globl	cerror

_msgsnd:
	.word 0x0000
	pushl	16(ap)
	pushl	12(ap)
	pushl	8(ap)
	pushl	4(ap)
	pushl	$3
	calls	$5,sys
	ret

_msgrcv:
	.word 0x0000
	pushl	20(sp)
	pushl	16(ap)
	pushl	12(ap)
	pushl	8(ap)
	pushl	4(ap)
	pushl	$2
	calls	$6,sys1
	ret

_msgctl:
	.word 0x0000
	pushl	12(ap)
	pushl	8(ap)
	pushl	4(ap)
	pushl	$1
	calls	$4,sys
	ret

_msgget:
	.word 0x0000
	pushl	8(ap)
	pushl	4(ap)
	pushl	$0
	calls	$3,sys1
	ret

sys:
	.word 0x0000
	chmk	$msg
	bcc	ok
	jmp	cerror
ok:
	clrl	r0
	ret

sys1:
	.word 0x0000
	chmk	$msg
	bcc	ok1
	jmp	cerror
ok1:
	ret
