.set	sem,53
.globl	_semctl
.globl	_semget
.globl	_semop
.globl	cerror

_semop:
	.word 0x0000
	pushl	12(ap)
	pushl	8(ap)
	pushl	4(ap)
	pushl	$2
	calls	$4,sys1
	ret

_semctl:
	.word 0x0000
	pushl	16(ap)
	pushl	12(ap)
	pushl	8(ap)
	pushl	4(ap)
	pushl	$1
	calls	$5,sys1
	ret

_semget:
	.word 0x0000
	pushl	12(ap)
	pushl	8(ap)
	pushl	4(ap)
	pushl	$0
	calls	$4,sys1
	ret

sys1:
	.word 0x0000
	chmk	$sem
	bcc	ok1
	jmp	cerror
ok1:
	ret
