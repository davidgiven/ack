.set	ioctl,54
.globl	_ioctl
.globl	cerror

_ioctl:
	.word	0x0000
	chmk	$ioctl
	bcc	ok
	jmp	cerror
ok:
	ret
