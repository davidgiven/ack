.globl	_stat
.set	stat,18

.align	1
_stat:
	.word	0x0000
	chmk	$stat
	bcc 	1f
	jmp 	errmon
1:
	clrl	r0
	ret
