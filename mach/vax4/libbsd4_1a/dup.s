.globl	_dup
.globl	_dup2
.set	dup,41

.align	1
_dup:
	.word	0x0000
	brb	1f

.align	1
_dup2:
	.word	0x0000
	bisb2	$0100,4(ap)
1:
	chmk	$dup
	bcc 	1f
	jmp 	errmon
1:
	ret
