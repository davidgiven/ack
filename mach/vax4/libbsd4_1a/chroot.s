.globl	_chroot
.set	chroot,61
 
.globl	errmon
_chroot:
	.word	0x0000
	chmk	$chroot
	bcc	1f
	jmp	errmon
1:
	ret
