.set	signal,48
.globl	_signal
.globl  cerror

_signal:
	.word	0x0000
	chmk	$signal
	bcc 	ok
	jmp 	cerror
ok:
	ret
