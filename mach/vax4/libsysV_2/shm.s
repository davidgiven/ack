.set	shm,52
.globl	_shmat
.globl	_shmctl
.globl	_shmdt
.globl	_shmget
.globl	cerror

_shmat:
	.word 0x0000
	pushl	12(ap)
	pushl	8(ap)
	pushl	4(ap)
	pushl	$0
	calls	$4,sys1
	ret

_shmdt:
	.word 0x0000
	pushl	4(ap)
	pushl	$2
	calls	$2,sys
	ret

_shmctl:
	.word 0x0000
	pushl	12(ap)
	pushl	8(ap)
	pushl	4(ap)
	pushl	$1
	calls	$4,sys
	ret

_shmget:
	.word 0x0000
	pushl	12(ap)
	pushl	8(ap)
	pushl	4(ap)
	pushl	$3
	calls	$4,sys1
	ret

sys:
	.word 0x0000
	chmk	$shm
	bcc	ok
	jmp	cerror
ok:
	clrl	r0
	ret

sys1:
	.word 0x0000
	chmk	$shm
	bcc	ok1
	jmp	cerror
ok1:
	ret
