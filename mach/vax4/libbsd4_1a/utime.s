.globl	_utime
.set	utime,30
 
.align	1
_utime:
	.word	0x0000
	chmk	$utime
	bcc	1f
	jmp	errmon
1:
	ret
