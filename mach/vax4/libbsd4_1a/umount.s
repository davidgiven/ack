.globl	_umount
.set	umount,22

.align	1
_umount:
	.word	0x0000
	chmk	$umount
	bcc 	1f
	jmp 	errmon
1:
	clrl	r0
	ret
