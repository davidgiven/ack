.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
shm = 52
.define	_shmat
.define	_shmctl
.define	_shmdt
.define	_shmget

_shmat:
	.data2 0x0000
	pushl	12(ap)
	pushl	8(ap)
	pushl	4(ap)
	pushl	$0
	calls	$4,Isys1
	ret

_shmdt:
	.data2 0x0000
	pushl	4(ap)
	pushl	$2
	calls	$2,Isys
	ret

_shmctl:
	.data2 0x0000
	pushl	12(ap)
	pushl	8(ap)
	pushl	4(ap)
	pushl	$1
	calls	$4,Isys
	ret

_shmget:
	.data2 0x0000
	pushl	12(ap)
	pushl	8(ap)
	pushl	4(ap)
	pushl	$3
	calls	$4,Isys1
	ret

Isys:
	.data2 0x0000
	chmk	$shm
	bcc	1f
	jmp	cerror
1:
	clrl	r0
	ret

Isys1:
	.data2 0x0000
	chmk	$shm
	bcc	1f
	jmp	cerror
1:
	ret
