.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
sem = 53
.define	_semctl
.define	_semget
.define	_semop

_semop:
	.data2 0x0000
	pushl	12(ap)
	pushl	8(ap)
	pushl	4(ap)
	pushl	$2
	calls	$4,Isys1
	ret

_semctl:
	.data2 0x0000
	pushl	16(ap)
	pushl	12(ap)
	pushl	8(ap)
	pushl	4(ap)
	pushl	$1
	calls	$5,Isys1
	ret

_semget:
	.data2 0x0000
	pushl	12(ap)
	pushl	8(ap)
	pushl	4(ap)
	pushl	$0
	calls	$4,Isys1
	ret

Isys1:
	.data2 0x0000
	chmk	$sem
	bcc	1f
	jmp	cerror
1:
	ret
