.set	chroot,61
.globl	_chroot
.globl	cerror

_chroot:
	.word	0x0000
	chmk	$chroot
	bcc	ok
	jmp	cerror
ok:
	ret
