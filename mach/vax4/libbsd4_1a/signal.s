.globl	_signal
.set	signal,48

.align	1
_signal:
	.word	0x0000
	chmk	$signal
	bcc	1f
	jmp	errmon
1:
	ret
