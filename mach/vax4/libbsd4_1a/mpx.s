.set	mpxcall,56
.globl	_mpxcall

.align	1
_mpxcall:
	.word	0x0000
	chmk	$mpxcall
	bcc 	1f
	jmp 	errmon
1:
	ret
