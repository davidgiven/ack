.sect .text; .sect .rom; .sect .data; .sect .bss
.sect .text
msg = 49
.define	_msgrcv
.define	_msgctl
.define	_msgsnd
.define	_msgget

_msgsnd:
	.data2 0x0000
	pushl	16(ap)
	pushl	12(ap)
	pushl	8(ap)
	pushl	4(ap)
	pushl	$3
	calls	$5,Isys
	ret

_msgrcv:
	.data2 0x0000
	pushl	20(sp)
	pushl	16(ap)
	pushl	12(ap)
	pushl	8(ap)
	pushl	4(ap)
	pushl	$2
	calls	$6,Isys1
	ret

_msgctl:
	.data2 0x0000
	pushl	12(ap)
	pushl	8(ap)
	pushl	4(ap)
	pushl	$1
	calls	$4,Isys
	ret

_msgget:
	.data2 0x0000
	pushl	8(ap)
	pushl	4(ap)
	pushl	$0
	calls	$3,Isys1
	ret

Isys:
	.data2 0x0000
	chmk	$msg
	bcc	1f
	jmp	cerror
1:
	clrl	r0
	ret

Isys1:
	.data2 0x0000
	chmk	$msg
	bcc	1f
	jmp	cerror
1:
	ret
