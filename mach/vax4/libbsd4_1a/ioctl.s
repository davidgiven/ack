.globl	_ioctl
.set	ioctl,54
 
.align	1
_ioctl:
	.word	0x0000
	chmk	$ioctl
	bcc	1f
	jmp	errmon
1:
	ret
